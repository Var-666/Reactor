//
// Created by Var on 25-8-2.
//

#include <utility>

#include "mylib/EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop, std::string name,int numThreads)
    :baseLoop_(baseLoop),
    name_(std::move(name)),
    started_(false),
    numThreads_(numThreads),
    next_(0){ }

EventLoopThreadPool::~EventLoopThreadPool() = default;

void EventLoopThreadPool::setThreadNum(const size_t numThreads) {
    numThreads_ = numThreads;
}

void EventLoopThreadPool::start(const ThreadInitCallback &cb) {
    started_ = true;

    for (int i = 0; i < numThreads_; i++) {
        std::string threadName = name_ + "-worker-" + std::to_string(i);
        auto thread = std::make_unique<EventLoopThread>(cb);
        EventLoop *loop = thread->startLoop();
        threads_.push_back(std::move(thread));
        loops_.push_back(loop);
    }

    if (numThreads_ == 0 && cb) {
        cb(baseLoop_);
    }
}

EventLoop * EventLoopThreadPool::getNextLoop() {
    if (loops_.empty()) {
        return baseLoop_;
    }

    EventLoop *loop = loops_[next_];
    ++next_;
    if (next_ >= static_cast<int>(loops_.size())) {
        next_ = 0;
    }
    return loop;
}

std::vector<EventLoop *> EventLoopThreadPool::getAllLoops() {
    if (loops_.empty()) {
        return {baseLoop_};
    }
    return loops_;
}

bool EventLoopThreadPool::started() const {
    return started_;
}

const std::string & EventLoopThreadPool::name() {
    return name_;
}
