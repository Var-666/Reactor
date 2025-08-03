//
// Created by Var on 25-8-3.
//

#include "TimerQueue.h"
#include "EventLoop.h"
#include <sys/timerfd.h>
#include <cstring>
#include <iostream>
#include <chrono>

static int createTimerfd() {
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd < 0) {
        std::cerr << "Failed to create timerfd\n";
        std::abort();
    }
    return timerfd;
}

static void resetTimerfd(int timerfd,Timer::Timestamp expiration) {
    itimerspec new_value{};
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(expiration - std::chrono::steady_clock::now());
    if (duration.count() < 1000000) {
        duration = std::chrono::milliseconds(1);
    }

    new_value.it_value.tv_sec = duration.count() / 1000000000;
    new_value.it_value.tv_nsec = duration.count() % 1000000000;
    ::timerfd_settime(timerfd, 0, &new_value,nullptr);
}

TimerQueue::TimerQueue(EventLoop *loop)
    :timerfd_(createTimerfd()),
    timerfdChannel_(loop,timerfd_),
    loop_(loop){
    timerfdChannel_.setReadCallback(std::bind(&TimerQueue::handleRead,this));
    timerfdChannel_.enableRead();
}

TimerQueue::~TimerQueue() {
    ::close(timerfd_);
}

void TimerQueue::addTimer(Timer::Callback cb, Timestamp when, double interval) {
    auto timer = std::make_shared<Timer>(std::move(cb),when,interval);
    loop_->runInLoopThread([this,timer]() {
        addTimerInLoop(timer);
    });
}

void TimerQueue::handleRead() {
    uint64_t expiration;
    ::read(timerfd_, &expiration, sizeof(expiration));

    auto now = std::chrono::steady_clock::now();
    auto expired = getExpired(now);

    for (auto & entry : expired) {
        entry.second->run();
    }

    restartTimers(expired,now);
}

void TimerQueue::addTimerInLoop(std::shared_ptr<Timer> timer) {
    bool earliestChanged = timers_.empty() || (timer->expiration() < timers_.begin()->first);
    timers_.insert({timer->expiration(),timer});
    if (earliestChanged) {
        resetTimerfd(timerfd_,timer->expiration());
    }
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now) {
    std::vector<Entry> expired;
    Entry dummy = {now ,nullptr};
    auto end = timers_.lower_bound(dummy);
    expired.insert(expired.end(),timers_.begin(),end);
    timers_.erase(timers_.begin(), end);
    return expired;
}

void TimerQueue::restartTimers(const std::vector<Entry> &expired, Timestamp now) {
    for (const auto& entry : expired) {
        if (entry.second->repeat()) {
            entry.second->restart(now);
            timers_.insert({entry.second->expiration(),entry.second});
        }
    }
    if (!timers_.empty()) {
        resetTimerfd(timerfd_,timers_.begin()->first);
    }
}

