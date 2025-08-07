//
// Created by Var on 25-8-3.
//

#ifndef TIMER_H
#define TIMER_H

#pragma once

#include <chrono>
#include <functional>

// •	回调函数（callback_）：定时到期时需要执行的任务。
// •	过期时间（expiration_）：这个定时器什么时候触发。
// •	周期间隔（interval_）：如果非零，表示这是一个周期性定时器，到期后会重新启动。

class Timer {
public:
    using Timestamp = std::chrono::steady_clock::time_point;
    using Callback = std::function<void()>;

    Timer(Callback cb,Timestamp expiration,double interval);
    ~Timer();

    void run() const;
    Timestamp expiration() const;
    bool repeat() const;
    void restart(Timestamp now);
private:
    Callback callback_;
    Timestamp expiration_;
    double interval_;
};



#endif //TIMER_H
