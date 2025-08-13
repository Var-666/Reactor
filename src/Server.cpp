//
// Created by Var on 25-7-25.
//

#include "mylib/Server.h"
#include <iostream>

Server::Server(EventLoop *loop, const InetAddress &listenAddr)
    :loop_(loop),
    acceptor_(loop,listenAddr),
    threadNum_(5),
    timeoutManager_(loop_,10){

    ServerWithThreadPools(loop);   //设置IO线程和业务线程

    acceptor_.setNewConnectionCallback(
        [this](int connfd,const InetAddress& peerAddr) {
            this->newConnection(connfd,peerAddr);
    });

}

Server::~Server() = default;

void Server::start() {
    // ioThreadPool_->setThreadNum(threadNum_);
    // ioThreadPool_->start();
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
    EventLoop *ioLoop = ioThreadPool_->getNextLoop();

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

    // 业务消息回调改造，异步执行业务逻辑
    conn->setMessageCallback(
        [this](const Connection::Ptr &conn, const std::string &msg) {
            businessThreadPool_->submit([this, conn, msg]() {
                if (messageCallback_) {
                    messageCallback_(conn, msg);  // 真正业务处理
                }
                // 这里可以根据业务需要调用 conn->send(...)
            });
        }
    );

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

void Server::ServerWithThreadPools(EventLoop *loop) {
    unsigned int threadNum = std::thread::hardware_concurrency();
    if (threadNum == 0) {
        threadNum = 4;
    }
    // IO线程数 = cpu核数 * 2 (可调)
    const size_t ioThreadNum = threadNum * 2;
    ioThreadPool_ = std::make_unique<EventLoopThreadPool>(loop,"ServerThreadPool");
    ioThreadPool_->setThreadNum(ioThreadNum);
    ioThreadPool_->start();

    // 业务线程池数 = cpu核数 * 3 (可调)
    size_t businessThreadNum = threadNum * 3;
    businessThreadPool_ = std::make_unique<ThreadPool>(businessThreadNum);
}
