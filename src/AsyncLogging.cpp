//
// Created by Var on 25-8-13.
//

#include "logger/AsyncLogging.h"
#include "logger/LogFile.h"
#include <chrono>
#include <iostream>


AsyncLogging::AsyncLogging(const std::string &basename, size_t rollSize, int flushInterval)
    :basename_(basename),
    rollSize_(rollSize),
    flashInterval_(flushInterval),
    running_(false){ }

AsyncLogging::~AsyncLogging() {
    if (running_) {
        stop();
    }
}

void AsyncLogging::append(const char *logline, size_t len) {
    std::lock_guard<std::mutex> lock(mutex_);
    currentBuffer_.append(logline, len);
    currentBuffer_.push_back('\n');  // 🔹保证每条日志末尾换行
    if (currentBuffer_.size() > 4096) {
        buffers_.push_back(std::move(currentBuffer_));
        currentBuffer_.clear();
        cond_.notify_one();
    }
}

void AsyncLogging::start() {
    running_ = true;
    thread_ = std::thread(&AsyncLogging::threadFunc, this);
}

void AsyncLogging::stop() {
    running_ = false;
    cond_.notify_all();
    if (thread_.joinable()) {
        thread_.join();
    }
}

void AsyncLogging::threadFunc() {
    LogFile output(basename_,rollSize_);
    while (running_) {
        std::vector<Buffer> buffersToWrite;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (buffers_.empty()) {
                cond_.wait_for(lock, std::chrono::milliseconds(flashInterval_));
            }
            buffers_.push_back(std::move(currentBuffer_));
            currentBuffer_.clear();
            buffersToWrite.swap(buffers_);
        }
        for (const auto &buffer : buffersToWrite) {
            output.append(buffer.c_str(),buffer.size());
        }
        output.flush();
    }
}
