#include "logger.h"

#include <errno.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "utils.h"

Logger *Logger::instance_ = nullptr;
Config *Logger::config_ = nullptr;

void Logger::ConfigInit(Config *cfg) { config_ = cfg; }

Logger::Logger() {
    if (config_ == nullptr) {
        fprintf(stderr, "Please init config first\n");
        exit(1);
    }
    if ((this->log_file_ = fopen(config_->logPath.c_str(), "a")) == NULL) {
        fprintf(stderr, "open %s failure: %s\n", config_->logPath.c_str(),
                strerror(errno));
    }
    /* 修改缓冲类型为行缓冲 */
    if (setvbuf(this->log_file_, NULL, _IOLBF, 0) != 0) {
        fprintf(stderr, "setvbuf error");
    }
}

Logger *Logger::GetInstance() {
    if (instance_ == nullptr) {
        instance_ = new Logger();
    }
    return instance_;
}

void Logger::Log(const char *LOG_LEVEL, const char *format, va_list arg) {
    char buf[BUFSIZ], fmt[BUFSIZ];
    sprintf(fmt, "[%s] %s \"%s\"", LOG_LEVEL, getTime().c_str(), format);
    vsnprintf(buf, BUFSIZ - 1, fmt, arg);
    fprintf(this->log_file_, "%s\n", buf);
    /* 同时输出到标准输出 */
    fprintf(stdout, "%s\n", buf);
}

void Logger::DEBUG(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    Logger::GetInstance()->Log("DEBUG", format, ap);
    va_end(ap);
}

void Logger::INFO(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    Logger::GetInstance()->Log("INFO", format, ap);
    va_end(ap);
}

void Logger::NOTICE(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    Logger::GetInstance()->Log("NOTICE", format, ap);
    va_end(ap);
}

void Logger::WARNING(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    Logger::GetInstance()->Log("WARNING", format, ap);
    va_end(ap);
}

void Logger::ERROR(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    Logger::GetInstance()->Log("ERROR", format, ap);
    va_end(ap);
    /* 发生 error 后退出程序 */
    exit(1);
}