//
// Created by Var on 25-8-3.
//

#ifndef TIMERQUEUE_H
#define TIMERQUEUE_H

#pragma once
#include <set>
#include <vector>
#include <unistd.h>

#include "Timer.h"
#include "Channel.h"

// TimerQueue 是定时器的管理器，维护一组 Timer，负责：
//     •	添加新的定时器；
//     •	管理所有定时器的过期时间顺序；
//     •	利用 Linux 的 timerfd 文件描述符，将定时器事件集成到 epoll 事件循环中；
//     •	触发过期的定时器，执行它们的回调；
//     •	对周期定时器自动重启。

class EventLoop;

class TimerQueue {
public:
    using Timestamp = Timer::Timestamp;

    explicit TimerQueue(EventLoop* loop);
    ~TimerQueue();

    void addTimer(Timer::Callback cb,Timestamp when,double interval);
private:
    using Entry = std::pair<Timestamp,std::shared_ptr<Timer>>;

    struct EntryCompare {
        bool operator()(const Entry& lhs,const Entry& rhs) const {
            return lhs.first < rhs.first;
        }
    };

    void handleRead();
    // void resetTimerfd(Timestamp expiration);
    void addTimerInLoop(std::shared_ptr<Timer> timer);
    std::vector<Entry> getExpired(Timestamp now);
    void restartTimers(const std::vector<Entry>& expired,Timestamp now);

    int timerfd_;
    Channel timerfdChannel_;
    std::set<Entry, EntryCompare> timers_;
    EventLoop* loop_;
};



#endif //TIMERQUEUE_H
