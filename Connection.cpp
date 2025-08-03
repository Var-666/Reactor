//
// Created by Var on 25-7-25.
//

#include "Connection.h"

#include <unistd.h>
#include <iostream>
#include <cerrno>     // for errno
#include <cstring>    // for strerror
#include <cassert>
#include <memory>

Connection::Connection(EventLoop *loop, int connfd) : loop_(loop),socket_(connfd),channel_(loop,connfd),state_(State::Connected){
    channel_.setReadCallback(std::bind(&Connection::handleRead,this));
    channel_.setWriteCallback(std::bind(&Connection::handleWrite, this));
    channel_.setCloseCallback(std::bind(&Connection::handleClose, this));
    channel_.setErrorCallback(std::bind(&Connection::handleError, this));
}

Connection::~Connection() {
    if (state_ == State::Connected) {
        handleClose();
    }
}

void Connection::setMessageCallback(MessageCallback cb) {
    messageCallback_ = std::move(cb);
}

void Connection::setCloseCallback(CloseCallback cb) {
    closeCallback_ = std::move(cb);
}

void Connection::send(const std::string &data) {
    if (state_ == State::Disconnected) {
        return;
    }
    if (loop_->isInLoopThread()) {
        sendInLoop(data);
    }else {
        loop_->runInLoopThread([self= shared_from_this(),data]() {
            self->sendInLoop(data);
        });
    }
}

void Connection::shutdown() {
    if (state_ == State::Connected) {
        state_ = State::Disconnected;
        if (loop_->isInLoopThread()) {
            shutdownInLoop();
        }else {
            loop_->runInLoopThread([self=shared_from_this()]() {
                self->shutdownInLoop();
            });
        }
    }
}

void Connection::enableRead() {
    channel_.enableRead();
}

EventLoop* Connection::getLoop() const {
    return loop_;
}

void Connection::connectEstablished() {
    assert(state_ == State::Connected);
    loop_->assertInLoopThread();

    channel_.tie(shared_from_this());
    channel_.enableRead();              // 真正开始监听读事件
}

void Connection::setActivityCallback(ActivityCallback cb) {
    updateActivityCallback_ = std::move(cb);
}

int Connection::fd() const {
    return socket_.fd();
}

void Connection::sendInLoop(const std::string &data) {
    if (state_ == State::Disconnected) {
        return;
    }
    ssize_t n = 0;
    if (outputBuffer_.empty()) {
        n = ::write(socket_.fd(), data.c_str(), data.size());
        if (n < 0) {
            if (errno != EWOULDBLOCK) {
                std::cerr << "Send error\n";
                handleError();
            }
            n = 0;
        }
    }
    if (n < static_cast<ssize_t>(data.size())) {
        outputBuffer_ +=data.substr(n);
        channel_.enableWrite();
    }
}

void Connection::shutdownInLoop() {
    if (channel_.isWriting()) {
        socket_.shutdownWrite();
    }
}

void Connection::handleRead() {
    char buf[4096];
    ssize_t n = ::read(socket_.fd(), buf, sizeof(buf));
    if (n > 0) {

        if (updateActivityCallback_) {
            updateActivityCallback_(fd());  // ✅ 更新活跃时间
        }

        inputBuffer_.append(buf, n);
        if (messageCallback_) {
            messageCallback_(shared_from_this(), inputBuffer_);
        }
        inputBuffer_.clear();   // 简化：一次性处理，真实项目可能需要保留
    }else if (n == 0) {
        handleClose();
    }else {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            handleError();
        }
    }
}

void Connection::handleWrite() {
    ssize_t n = ::write(socket_.fd(), outputBuffer_.data(), outputBuffer_.size());
    if (n > 0) {
        outputBuffer_.erase(0,n);
        if (outputBuffer_.empty()) {
            channel_.disableWrite();
            if (state_ == State::Disconnected) {
                shutdownInLoop();
            }
        }
    }else {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            handleError();
        }
    }

}

void Connection::handleClose() {
    if (state_ == State::Disconnected) {
        // 已经关闭，直接返回，避免重复处理
        return;
    }
    std::cout << "handleClose called for fd=" << fd() << std::endl;
    state_ = State::Disconnected;
    loop_->removeChannel(&channel_);
    std::cout << "Channel removed for fd=" << fd() << std::endl;
    if (closeCallback_) {
        closeCallback_(shared_from_this());
    }
}

void Connection::handleError() {
    std::cerr << "Connection fd=" << fd() << " error: " << strerror(errno) << std::endl;
    handleClose();
}
