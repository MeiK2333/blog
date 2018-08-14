#include <unistd.h>
#include <iostream>

#include "config.h"
#include "logger.h"
#include "master.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " [blog.conf]" << std::endl;
        return 1;
    }
    /* 读取配置文件 */
    Config config(argv[1]);

    /* 设置 Logger */
    Logger::ConfigInit(&config);
    Logger::INFO("run server on pid %d", getpid());

    /* 开始运行服务进程 */
    Master master(&config);
    master.startup();

    return 0;
}