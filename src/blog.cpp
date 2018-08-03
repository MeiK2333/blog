#include <unistd.h>
#include <iostream>

#include "config.h"
#include "logger.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " [blog.conf]" << std::endl;
        return 1;
    }
    Config *config = new Config(argv[1]);

    Logger::configInit(config);
    Logger::info("run server on pid " + std::to_string(getpid()));
    return 0;
}