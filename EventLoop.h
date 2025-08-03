//
// Created by Var on 25-7-25.
//

#ifndef EVENTLOOP_H
#define EVENTLOOP_H
#pragma once
#include <atomic>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>

#include "Epoller.h"
#include "Channel.h"
#include "TimerQueue.h"

class EventLoop {
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);

    void assertInLoopThread() const;
    bool isInLoopThread() const;
    void runInLoopThread(std::function<void()> cb);

    void wakeup() const;
    void handleRead() const;

    void runAfter(double delay,std::function<void()> cb) const;
    void runEvery(double interval,std::function<void()> cb) const;

private:
    void doPendingFunctors();
    std::atomic<bool> looping_;
    std::atomic<bool> quit_;

    int wakeupFd_;
    std::unique_ptr<Channel> wakeupchannel_;

    Epoller epoller_;
    std::vector<Channel *> activeChannels_;

    std::mutex mutex_;
    std::thread::id threadID_;
    std::queue<std::function<void()>> pendingFunctors_;
    std::unique_ptr<TimerQueue> timerQueue_;
};


#endif //EVENTLOOP_H
