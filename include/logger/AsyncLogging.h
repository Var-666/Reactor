//
// Created by Var on 25-8-13.
//

#ifndef ASYNCLOGGING_H
#define ASYNCLOGGING_H


#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <string>
#include <atomic>
#include "LogFile.h"

class AsyncLogging {
public:
    AsyncLogging(const std::string& basename, size_t rollSize, int flushInterval = 3);
    ~AsyncLogging();

    void append(const char* logline, size_t len);
    void start();
    void stop();
private:
    void threadFunc();

    using Buffer = std::string;

    const std::string basename_;     //日志文件基础名，用于生成日志文件名。
    const size_t rollSize_;          //日志滚动大小，达到该大小就创建新文件
    const int flashInterval_;        //刷新间隔（秒）

    std::atomic<bool> running_;
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;

    Buffer currentBuffer_;
    std::vector<Buffer> buffers_;
};



#endif //ASYNCLOGGING_H
