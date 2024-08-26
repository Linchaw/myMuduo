#include "Logger.h"

#include "TimeStamp.h"

Logger &Logger::instance()
{
    static Logger logger;
    return logger;
}

Logger::Logger()
{
    logFile.open("log/run.log");
    if (!logFile)
    {
        std::cerr << "Unable to open file" << std::endl;
        // 可以抛出异常或设置一个错误标志
        throw std::ios_base::failure("Failed to open run.log");
    }
}

Logger::~Logger()
{
    logFile.close();
}

void Logger::log(const std::string&& msg, int level)
{
    // auto now = std::chrono::system_clock::now();
    // std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

    std::lock_guard<std::mutex> lock(mtx);
    switch (level)
    {
    case INFO:
        logFile << "[INFO] \t" ;
        break;
    case ERROR:
        logFile << "[ERROR] \t";
        break;
    case FATAL:
        logFile << "[FATAL] \t";
        break;
    case DEBUG:
        logFile << "[DEBUG] \t";
        break;
    default:
        break;
    }
    logFile << msg << "\t SPID:" << syscall(SYS_gettid) <<"\t time: " << TimeStamp::now().toString() << std::endl;
}