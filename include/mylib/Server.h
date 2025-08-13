//
// Created by Var on 25-7-25.
//

#ifndef SERVER_H
#define SERVER_H
#pragma once

#include <unordered_map>
#include <memory>
#include <functional>

#include "EventLoop.h"
#include "Acceptor.h"
#include "Connection.h"
#include "EventLoopThreadPool.h"
#include "ConnectionTimeoutManager.h"
#include "ThreadPool.h"

class Server {
public:
    using ConnectionCallback = std::function<void(const Connection::Ptr&)>;
    using MessageCallback = Connection::MessageCallback;

    Server(EventLoop* loop, const InetAddress& listenAddr);
    ~Server();

    void start();

    void setNumThreads(int numThreads);
    void setConnectionCallback(const ConnectionCallback& cb);
    void setMessageCallback(const MessageCallback& cb);

private:
    void newConnection(int connfd, const InetAddress& peerAddr);
    void removeConnection(const Connection::Ptr& conn);
    void ServerWithThreadPools(EventLoop* loop);   //配置IO线程池和业务线程池

    EventLoop* loop_;
    Acceptor acceptor_;
    std::unique_ptr<EventLoopThreadPool> ioThreadPool_;
    std::unique_ptr<ThreadPool> businessThreadPool_;
    int threadNum_;

    std::unordered_map<int, Connection::Ptr> connections_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    ConnectionTimeoutManager timeoutManager_;
};



#endif //SERVER_H
