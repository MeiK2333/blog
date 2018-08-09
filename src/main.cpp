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
    Config *config = new Config(argv[1]);

    /* 设置 Logger */
    Logger::configInit(config);
    Logger::info("run server on pid " + std::to_string(getpid()));

    /* 开始运行服务进程 */
    Master *master = new Master(config);
    master->startup();

    delete config;
    delete master;
    return 0;
}