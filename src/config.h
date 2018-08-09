#ifndef BLOG_CONFIG_H
#define BLOG_CONFIG_H

#include <string>

enum logLevelEnum {
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_NOTICE,
    LOG_WARNING,
    LOG_ERROR
};

class Config {
   public:
    /* 从配置文件中读取 config */
    Config(std::string);
    ~Config(){};

    void show();

    int listenPort;
    int cpuCount;
    std::string rootPath;
    logLevelEnum logLevel;
    std::string logPath;
};

#endif