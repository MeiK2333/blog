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
    Logger::INFO("server listen port %d", this->config->listenPort);

    /* 依次启动每个 work 子进程 */
    for (i = 0; i < this->config->cpuCount; i++) {
        pid_t pid;
        if ((pid = fork()) < 0) {
            Logger::ERROR("fork failure!");
        } else if (pid == 0) {
            Logger::DEBUG("run work process %d on %d", i, getpid());

            cd(this->config->rootPath);

            Work work(listenfd);
            work.start();

            exit(1);
        } else {
            this->children[i] = pid;
            /* 为子进程绑定 CPU */
            set_affnity(pid, i);
            Logger::DEBUG("set_affnity pid %d to CPU %d", pid, i);
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
        Logger::ERROR("socket failure!");
    }

    bzero(&name, sizeof(name));
    name.sin_family = AF_INET;
    name.sin_port = htons(this->config->listenPort);
    name.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(httpd, (struct sockaddr *)&name, sizeof(name)) < 0) {
        Logger::ERROR("bind failure!");
    }

    return httpd;
}