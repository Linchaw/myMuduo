#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <sys/syscall.h>
#include <unistd.h>
#include <chrono>
#include <iomanip>
#include <mutex>

#include "noncopyable.h"

#define LOG_INFO(logmsgFormat, ...) \
do{\
    Logger &logger = Logger::instance(); \
    logger.setLogLevel(INFO); \
    char buf[1024] = {0}; \
    snprintf(buf, 1024, logmsgFormat, ##VA_ARGS); \
    logger.log(buf); \
}while(0)

#define LOG_ERROR(logmsgFormat, ...) \
do{\
    Logger &logger = Logger::instance(); \
    logger.setLogLevel(ERROR); \
    char buf[1024] = {0}; \
    snprintf(buf, 1024, logmsgFormat, ##VA_ARGS); \
    logger.log(buf); \
}while(0)

#define LOG_FATAL(logmsgFormat, ...) \
do{\
    Logger &logger = Logger::instance(); \
    logger.setLogLevel(FATAL); \
    char buf[1024] = {0}; \
    snprintf(buf, 1024, logmsgFormat, ##VA_ARGS); \
    logger.log(buf); \
}while(0)

#ifdef MUDEBUG
#define LOG_DEBUG(logmsgFormat, ...) \
do{\
    Logger &logger = Logger::instance(); \
    logger.setLogLevel(DEBUG); \
    char buf[1024] = {0}; \
    snprintf(buf, 1024, logmsgFormat, ##VA_ARGS); \
    logger.log(buf); \
}while(0)
#else
    #define LOG_DEBUG(logmsgFormat, ...)
#endif

enum LoggerLevel
{
    INFO,
    ERROR,
    FATAL,
    DEBUG
};

class Logger : noncopyable
{
public:
    static Logger &instance();
    void log(const std::string&& msg, int level=0);
    Logger();
    ~Logger();

private:
    std::ofstream logFile;
    std::mutex mtx;

};