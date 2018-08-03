#include "logger.h"

#include <cstdlib>
#include <iostream>

#include "utils.h"

Logger *Logger::instance = nullptr;
Config *Logger::config = nullptr;

void Logger::configInit(Config *cfg) { config = cfg; }

Logger::Logger() {
    if (config == nullptr) {
        std::cerr << "Please init config first" << std::endl;
        exit(1);
    }
    logStream.open(config->logPath, std::ios_base::app);
}

Logger *Logger::getInstance() {
    if (instance == nullptr) {
        instance = new Logger();
    }
    return instance;
}

void Logger::log(std::string log) {
    logStream << log << std::endl;
    /* 同时输出到标准输出 */
    std::cout << log << std::endl;
}

void Logger::debug(std::string msg) {
    if (config->logLevel > LOG_DEBUG) {
        return;
    }
    std::string logStr = "[DEBUG] " + getTime() + " \"" + msg + "\"";
    Logger *logger = Logger::getInstance();
    logger->log(logStr);
}

void Logger::info(std::string msg) {
    if (config->logLevel > LOG_INFO) {
        return;
    }
    std::string logStr = "[INFO] " + getTime() + " \"" + msg + "\"";
    Logger *logger = Logger::getInstance();
    logger->log(logStr);
}

void Logger::notice(std::string msg) {
    if (config->logLevel > LOG_NOTICE) {
        return;
    }
    std::string logStr = "[NOTICE] " + getTime() + " \"" + msg + "\"";
    Logger *logger = Logger::getInstance();
    logger->log(logStr);
}

void Logger::warning(std::string msg) {
    if (config->logLevel > LOG_WARNING) {
        return;
    }
    std::string logStr = "[WARNING] " + getTime() + " \"" + msg + "\"";
    Logger *logger = Logger::getInstance();
    logger->log(logStr);
}

void Logger::error(std::string msg) {
    if (config->logLevel > LOG_ERROR) {
        return;
    }
    std::string logStr = "[ERROR] " + getTime() + " \"" + msg + "\"";
    Logger *logger = Logger::getInstance();
    logger->log(logStr);
}