#ifndef BLOG_LOGGER_H
#define BLOG_LOGGER_H

#include <stdarg.h>
#include <cstdio>
#include <fstream>
#include <string>

#include "config.h"

class Logger {
   public:
    /* 打印对应类型的日志 */
    static void DEBUG(const char *, ...);
    static void INFO(const char *, ...);
    static void NOTICE(const char *, ...);
    static void WARNING(const char *, ...);
    static void ERROR(const char *, ...);
    /* 打印一条 LOG */
    void Log(const char *, const char *, va_list);

   public:
    /* 获取单例模式的实例 */
    static Logger *GetInstance();
    /* 初始化配置 */
    static void ConfigInit(Config *);

   private:
    static Config *config_;
    std::ofstream logStream;
    FILE *log_file_;

    Logger();

    static Logger *instance_;
};

#endif