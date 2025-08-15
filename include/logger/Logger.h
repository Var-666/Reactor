//
// Created by Var on 25-8-13.
//

#ifndef LOGGER_H
#define LOGGER_H

#pragma once
#include <string>
#include <memory>

#include "AsyncLogging.h"


enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

class Logger {
public:
    // 获取全局单例 Logger
    static Logger& instance();

    // 设置日志级别
    void setLogLevel(LogLevel level);

    // 写日志（自动带换行）
    void log(LogLevel level, const char* file, int line, const char* fmt, ...);

    // 设置异步日志器（必须先调用一次）
    void setAsyncLogger(std::shared_ptr<AsyncLogging> asyncLogger);
private:
    Logger();
    ~Logger();

    LogLevel logLevel_;
    std::shared_ptr<AsyncLogging> asyncLogger_;
};


// 方便宏定义，自动带文件名和行号
#define LOG_TRACE(fmt, ...) Logger::instance().log(LogLevel::TRACE, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) Logger::instance().log(LogLevel::DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt,  ...) Logger::instance().log(LogLevel::INFO,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt,  ...) Logger::instance().log(LogLevel::WARN,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) Logger::instance().log(LogLevel::ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) Logger::instance().log(LogLevel::FATAL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)


#endif //LOGGER_H
