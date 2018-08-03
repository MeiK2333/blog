#ifndef BLOG_LOGGER_H
#define BLOG_LOGGER_H

#include <fstream>
#include <string>

#include "config.h"

class Logger {
   public:
    /* 打印对应类型的日志 */
    static void debug(std::string);
    static void info(std::string);
    static void notice(std::string);
    static void warning(std::string);
    static void error(std::string);
    /* 打印一条 LOG */
    void log(std::string);

   public:
    /* 获取单例模式的实例 */
    static Logger *getInstance();
    /* 初始化配置 */
    static void configInit(Config *);

   private:
    static Config *config;
    std::ofstream logStream;

    Logger();

    static Logger *instance;
};

#endif