//
// Created by Var on 25-8-1.
//

#ifndef EVENTLOOPTHREAD_H
#define EVENTLOOPTHREAD_H

#pragma once
#include <thread>
#include <condition_variable>
#include <functional>
#include <mutex>

class EventLoop;

class EventLoopThread {
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;

    explicit EventLoopThread(const ThreadInitCallback& cb);
    ~EventLoopThread();

    EventLoop* startLoop();
private:
    void threadFunc();

    EventLoop *loop_;
    bool exiting_;
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable condition_;

    ThreadInitCallback callback_;
};



#endif //EVENTLOOPTHREAD_H
