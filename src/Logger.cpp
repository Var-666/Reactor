//
// Created by Var on 25-8-13.
//

#include "logger/Logger.h"
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <iostream>
#include <mutex>
#include <iostream>

Logger & Logger::instance() {
    static Logger logger;
    return logger;
}

void Logger::setLogLevel(LogLevel level) {
    logLevel_ = level;
}

void Logger::log(LogLevel level, const char *file, int line, const char *fmt, ...) {
    if (level < logLevel_) {
        return;
    }

    /*	•	获取当前时间 now。
        •	localtime_r 将时间转换为本地时间的 tm 结构（线程安全）。
        •	strftime 格式化为 "YYYY-MM-DD HH:MM:SS" 字符串。
        •	buffer[1024] 用于存储完整日志信息（前缀 + 格式化消息）。
     */
    char buffer[1024];
    char timebuf[32];
    time_t now = time(nullptr);
    tm tm_time{};
    localtime_r(&now, &tm_time);
    strftime(timebuf,sizeof(timebuf),"%Y-%m-%d %H:%M:%S", &tm_time);

    const char* levelStr[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
    int idx = static_cast<int>(level);
    int n = snprintf(buffer,sizeof(buffer),"[%s] [%s] [%s:%d] ",timebuf,levelStr[idx],file,line);

    va_list args;
    va_start(args, fmt);
    n += vsnprintf(&buffer[n],sizeof(buffer)-n, fmt, args);
    buffer[n++] = '\n';  // 保证每条日志换行
    va_end(args);

    if (asyncLogger_) {
        asyncLogger_->append(buffer, n);
    }else {
        fwrite(buffer, 1, n, stdout);
        fwrite("\n", 1, 1, stdout);
    }

}

void Logger::setAsyncLogger(std::shared_ptr<AsyncLogging> asyncLogger) {
    asyncLogger_ = std::move(asyncLogger);
}

Logger::Logger():logLevel_(LogLevel::INFO) {
}

Logger::~Logger() = default;