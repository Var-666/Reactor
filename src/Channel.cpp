//
// Created by Var on 25-7-25.
//

#include "mylib/Channel.h"
#include "mylib/EventLoop.h"
#include <sys/epoll.h>
#include <cstdint>


Channel::Channel(EventLoop* loop,int fd):fd_(fd),events_(0),revents_(0),loop_(loop){

}

Channel::~Channel() {
}

void Channel::handleEvent() {
    if (tied_) {
        std::shared_ptr<void> guard = tie_.lock();
        if (guard) {
            handleEventWithGuard();
        } // 如果对象已经析构，直接跳过
    } else {
        handleEventWithGuard();
    }
}

void Channel::handleEventWithGuard() {
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
        if (closeCallback_) closeCallback_();
    }

    if (revents_ & EPOLLERR) {
        if (errorCallback_) errorCallback_();
    }

    if (revents_ & (EPOLLIN | EPOLLPRI)) {
        if (readCallback_) readCallback_();
    }

    if (revents_ & EPOLLOUT) {
        if (writeCallback_) writeCallback_();
    }
}

void Channel::setReadCallback( std::function<void()> callback) {
    readCallback_ = std::move(callback);
}

void Channel::setWriteCallback(std::function<void()> callback) {
    writeCallback_ = std::move(callback);
}

void Channel::setCloseCallback(std::function<void()> callback) {
    closeCallback_ = std::move(callback);
}

void Channel::setErrorCallback(std::function<void()> callback) {
    errorCallback_ = std::move(callback);
}

void Channel::tie(const std::shared_ptr<void>& obj) {
    tie_ = obj;
    tied_ = true;
}

void Channel::enableRead() {
    events_ |= EPOLLIN;
    loop_->updateChannel(this);
}

void Channel::enableWrite() {
    events_ |= EPOLLOUT;
    loop_->updateChannel(this);
}

void Channel::disableWrite() {
    events_ &= ~EPOLLOUT;
    loop_->updateChannel(this);
}

void Channel::disableAll() {
    events_ = 0;
    loop_->updateChannel(this);
}

bool Channel::isWriting() {
    return events_ & EPOLLOUT;
}

int Channel::fd() const {
    return fd_;
}

uint32_t Channel::events() const {
    return events_;
}

void Channel::setEvents(uint32_t revents) {
    revents_ = revents;
}
