//
// Created by Var on 25-7-25.
//

#include "mylib/Connection.h"
#include "protocol/LengthHeaderProtocol.h"

#include <unistd.h>
#include <iostream>
#include <cerrno>     // for errno
#include <cstring>    // for strerror
#include <cassert>
#include <memory>

Connection::Connection(EventLoop *loop, int connfd)
    : loop_(loop),
    socket_(connfd),
    channel_(loop,connfd),
    state_(State::Connected){
    channel_.setReadCallback(std::bind(&Connection::handleRead,this));
    channel_.setWriteCallback(std::bind(&Connection::handleWrite, this));
    channel_.setCloseCallback(std::bind(&Connection::handleClose, this));
    channel_.setErrorCallback(std::bind(&Connection::handleError, this));

    protocol_ = std::make_shared<LengthHeaderProtocol>();
    codec_ = std::make_shared<Codec>(protocol_);

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
    std::string packet;
    if (codec_) {
        packet = codec_->encode(data);  // 先编码
    } else {
        packet = data;                  // 没有协议，直接发数据
    }
    if (loop_->isInLoopThread()) {
        sendInLoop(packet);
    }else {
        loop_->runInLoopThread([self= shared_from_this(),packet]() {
            self->sendInLoop(packet);
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

void Connection::setCodec(std::shared_ptr<Codec> codec) {
    codec_ = std::move(codec);
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
    if (outputBuffer_.readableBytes() == 0) {
        n = ::write(socket_.fd(), data.data(), data.size());
        if (n >= 0) {
            if (static_cast<size_t>(n) == data.size()) {
                // 全部写完，直接返回
                return;
            }
        } else {
            n = 0;
            if (errno != EWOULDBLOCK && errno != EAGAIN) {
                std::cerr << "send error, fd=" << socket_.fd() << std::endl;
                handleError();
                return;
            }
        }
    }
    // 剩余数据写不完，追加到 outputBuffer_，等待写事件触发继续写
    outputBuffer_.append(data.data() + n, data.size() - n);
    channel_.enableWrite();
}

void Connection::shutdownInLoop() {
    if (channel_.isWriting()) {
        socket_.shutdownWrite();
    }
}

void Connection::handleRead() {
    int savedErrno = 0;
    ssize_t n = inputBuffer_.readFd(socket_.fd(), &savedErrno);
    if (n > 0) {
        if (updateActivityCallback_) {
            updateActivityCallback_(fd());  // ✅ 更新活跃时间
        }

        if (codec_) {
            std::vector<std::string> messages;
            bool hasMsg = codec_->decode(inputBuffer_, messages);

            if (hasMsg) {
                for (auto& msg : messages) {
                    if (messageCallback_) {
                        messageCallback_(shared_from_this(), msg);
                    }
                }
            }
        }else {
            if (messageCallback_) {
                std::string msg = inputBuffer_.retrieveAllAsString();
                messageCallback_(shared_from_this(), msg);
            }
        }
    }else if (n == 0) {
        handleClose();
    }else {
        if (savedErrno != EAGAIN && errno != EWOULDBLOCK) {
            handleError();
        }
    }
}

void Connection::handleWrite() {
    ssize_t n = ::write(socket_.fd(), outputBuffer_.peek(), outputBuffer_.readableBytes());
    if (n > 0) {
        outputBuffer_.retrieve(n);
        if (outputBuffer_.readableBytes() == 0) {
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
