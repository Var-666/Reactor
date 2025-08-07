//
// Created by Var on 25-7-25.
//

#include "mylib/Server.h"
#include <iostream>

Server::Server(EventLoop *loop, const InetAddress &listenAddr)
    :loop_(loop),
    acceptor_(loop,listenAddr),
    threadPool_(std::make_unique<EventLoopThreadPool>(loop,"ServerThreadPool")),
    timeoutManager_(loop_,10){

    acceptor_.setNewConnectionCallback(
        [this](int connfd,const InetAddress& peerAddr) {
            this->newConnection(connfd,peerAddr);
    });

}

Server::~Server() {
}

void Server::start() {
    threadPool_->setThreadNum(threadNum_);
    threadPool_->start();
    acceptor_.listen();
}

void Server::setNumThreads(int numThreads) {
    threadNum_ = numThreads;
}

void Server::setConnectionCallback(const ConnectionCallback &cb) {
    connectionCallback_ = cb;
}

void Server::setMessageCallback(const MessageCallback &cb) {
    messageCallback_ = cb;
}

void Server::newConnection(int connfd, const InetAddress &peerAddr) {
    EventLoop *ioLoop = threadPool_->getNextLoop();

    auto conn = std::make_shared<Connection>(ioLoop,connfd);
    connections_[connfd] = conn;

    // 1. 添加连接到超时管理器
    timeoutManager_.addConnection(conn);

    // 2. 设置活动回调（用于刷新连接时间）
    conn->setActivityCallback(
        [this](int fd) {
            timeoutManager_.updateConnectionActivity(fd);
        });

    if (connectionCallback_) {
        connectionCallback_(conn);
    }

    conn->setMessageCallback(messageCallback_);

    conn->setCloseCallback(
        [this](const Connection::Ptr &conn) {
            this->removeConnection(conn);
        });

    ioLoop->runInLoopThread([conn]() {
        conn->connectEstablished();
    });

}

void Server::removeConnection(const Connection::Ptr &conn) {
    EventLoop *ioLoop = conn->getLoop();
    ioLoop->runInLoopThread(
        [this,conn]() {
            connections_.erase(conn->fd());
            timeoutManager_.removeConnection(conn->fd()); // ✅ 移除连接
        });
}
