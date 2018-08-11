#include "master.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <string>

#include "logger.h"
#include "utils.h"
#include "work.h"

Master::Master(Config *config) {
    this->config = config;
    this->children = new pid_t[this->config->cpuCount];
}

Master::~Master() { delete this->children; }

void Master::startup() {
    int listenfd, i;
    /* 开始监听端口 */
    listenfd = this->socket_bind();
    listen(listenfd, 5);
    Logger::info("server listen port " +
                 std::to_string(this->config->listenPort));

    /* 依次启动每个 work 子进程 */
    for (i = 0; i < this->config->cpuCount; i++) {
        pid_t pid;
        if ((pid = fork()) < 0) {
            Logger::error("fork failure!");
        } else if (pid == 0) {
            Logger::debug("run work process " + std::to_string(i) + " on " +
                          std::to_string(getpid()));

            Work work(listenfd);
            work.start();

            exit(1);
        } else {
            this->children[i] = pid;
            /* 为子进程绑定 CPU */
            set_affnity(pid, i);
            Logger::debug("set_affnity pid " + std::to_string(pid) +
                          " to CPU " + std::to_string(i));
        }
    }
    pause();
    delete this->children;
}

/**
 * 监听指定端口的连接
 * */
int Master::socket_bind() {
    int httpd = 0;
    struct sockaddr_in name;

    if ((httpd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        Logger::error("socket failure!");
    }

    bzero(&name, sizeof(name));
    name.sin_family = AF_INET;
    name.sin_port = htons(this->config->listenPort);
    name.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(httpd, (struct sockaddr *)&name, sizeof(name)) < 0) {
        Logger::error("bind failure!");
    }

    return httpd;
}