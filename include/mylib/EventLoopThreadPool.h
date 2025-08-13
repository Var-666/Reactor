//
// Created by Var on 25-8-2.
//

#ifndef EVENTLOOPTHREADPOOL_H
#define EVENTLOOPTHREADPOOL_H

#pragma once

#include <vector>
#include <memory>
#include <string>
#include "EventLoopThread.h"

class EventLoopThreadPool {
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;

    EventLoopThreadPool(EventLoop *baseLoop, std::string name,int numThreads = 2);
    EventLoopThreadPool(const EventLoopThreadPool&) = delete;
    EventLoopThreadPool& operator=(const EventLoopThreadPool&) = delete;
    ~EventLoopThreadPool();

    void setThreadNum(size_t numThreads);
    void start(const ThreadInitCallback &cb = ThreadInitCallback());

    EventLoop *getNextLoop();
    std::vector<EventLoop *> getAllLoops();

    bool started() const;
    const std::string& name();

private:
    EventLoop *baseLoop_;
    std::string name_;
    bool started_;
    size_t numThreads_;
    int next_;

    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop *> loops_;
};



#endif //EVENTLOOPTHREADPOOL_H
