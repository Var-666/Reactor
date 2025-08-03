//
// Created by Var on 25-7-25.
//

#include "EventLoop.h"
#include "Channel.h"
#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/eventfd.h>

EventLoop::EventLoop():looping_(false),quit_(false),threadID_(std::this_thread::get_id()) {
    wakeupFd_ = ::eventfd(0,EFD_NONBLOCK | EFD_CLOEXEC);
    if (wakeupFd_ <= 0) {
        perror("eventfd error");
        exit(1);
    }

    wakeupchannel_ = std::make_unique<Channel>(this,wakeupFd_);
    wakeupchannel_->setReadCallback([this]() {
        handleRead();
    });
    wakeupchannel_->enableRead();

}

EventLoop::~EventLoop() {
    ::close(wakeupFd_);
    quit_ = true;
}

void EventLoop::loop() {
    if (looping_) {
        std::cerr << "EventLoop is already running!" << std::endl;
        return;
    }

    looping_ = true;
    quit_ = false;

    while(!quit_) {
        std::vector<Channel*> activeChannels_= epoller_.poll(1000);
        for (auto channel : activeChannels_) {
            channel->handleEvent();
        }
        doPendingFunctors();
    }
    looping_ = false;
}

void EventLoop::quit() {
    quit_ = true;
}

void EventLoop::updateChannel(Channel *channel) {
    epoller_.updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
    epoller_.removeChannel(channel);
}

void EventLoop::assertInLoopThread() const {
    if (!isInLoopThread()) {
        std::cerr << "Not in loop thread!\n";
        abort();
    }
}

bool EventLoop::isInLoopThread() const {
    return std::this_thread::get_id() == threadID_;
}

void EventLoop::runInLoopThread(std::function<void()> cb) {
    if (isInLoopThread()) {
        cb();
    }else {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            pendingFunctors_.push(std::move(cb));
        }
        wakeup();
    }
}

void EventLoop::wakeup() const {
    uint64_t one = 1;
    ssize_t n = ::write(wakeupFd_, &one, sizeof (one));
    if (n != sizeof (one)) {
        perror("EventLoop::wakeup write error");
    }
}
void EventLoop::handleRead() const {
    uint64_t one = 1;
    ssize_t n = ::read(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one)) {
        perror("EventLoop::handleRead read error");
    }
}

void EventLoop::doPendingFunctors() {
    std::queue<std::function<void()>> Functors_;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        std::swap(Functors_, pendingFunctors_);
    }
    while (!Functors_.empty()) {
        Functors_.front()();
        Functors_.pop();
    }
}
