//
// Created by Var on 25-8-1.
//

#include "mylib/EventLoopThread.h"

#include "mylib/EventLoop.h"

EventLoopThread::EventLoopThread(const ThreadInitCallback &cb):loop_(nullptr),exiting_(false),thread_(),callback_(cb) {

}

EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    if (loop_ != nullptr) {
        loop_->quit();
        thread_.join();
    }
}

EventLoop * EventLoopThread::startLoop() {
    thread_ = std::thread(&EventLoopThread::threadFunc, this);

    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait(lock,[this]() {
        return loop_ != nullptr;
    });

    return loop_;
}

void EventLoopThread::threadFunc() {
    EventLoop loop;
    if (callback_) {
        callback_(&loop);
    }

    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = &loop;
        condition_.notify_one();
    }

    loop.loop();

    loop_ = nullptr;
}
