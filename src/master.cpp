#include "master.h"

#include <unistd.h>
#include <string>

#include "logger.h"
#include "utils.h"
#include "work.h"

Master::Master(Config *config) {
    this->config = config;
    this->children = new pid_t[this->config->cpuCount];
}

void Master::startup() {
    int i;
    /* 依次启动每个 work 子进程 */
    for (i = 0; i < this->config->cpuCount; i++) {
        pid_t pid;
        if ((pid = fork()) < 0) {
            Logger::error("fork failure!");
        } else if (pid == 0) {
            Logger::info("run work process " + std::to_string(i) + " on " +
                         std::to_string(getpid()));
            Work *work = new Work();
            work->start();
            delete work;
            exit(1);
        } else {
            this->children[i] = pid;
            /* 为子进程绑定 CPU */
            set_affnity(pid, i);
        }
    }
    delete this->children;
}