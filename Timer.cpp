//
// Created by Var on 25-8-3.
//

#include "Timer.h"

#include <utility>

Timer::Timer(Callback cb, Timestamp expiration, double interval):callback_(std::move(cb)),expiration_(expiration),interval_(interval){
}

Timer::~Timer() = default;

void Timer::run() const {
    callback_();
}

Timer::Timestamp Timer::expiration() const {
    return expiration_;
}

bool Timer::repeat() const {
    return interval_ > 0;
}

void Timer::restart(Timestamp now) {
    if (repeat()) {
        expiration_ = now + std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                                 std::chrono::duration<double>(interval_));
    }
}

