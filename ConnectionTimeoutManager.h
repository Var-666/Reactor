//
// Created by Var on 25-8-3.
//

#ifndef CONNECTIONTIMEOUTMANAGER_H
#define CONNECTIONTIMEOUTMANAGER_H

#pragma once

#include <memory>
#include <chrono>
#include <unordered_map>
#include <functional>

#include "Connection.h"
#include "EventLoop.h"

class ConnectionTimeoutManager {
public:
    using Timestamp = std::chrono::steady_clock::time_point;

    ConnectionTimeoutManager(EventLoop *loop,int timeoutSecond);
    ~ConnectionTimeoutManager();

    void addConnection(std::shared_ptr<Connection> conn);
    void updateConnectionActivity(int connfd);
    void removeConnection(int connfd);
private:
    void checkTimeout();

    struct connInfo{
        std::weak_ptr<Connection> conn;
        Timestamp lastActive;
    };

    EventLoop *loop_;
    int timeoutSeconds_;
    std::unordered_map<int, connInfo> connections_;
};



#endif //CONNECTIONTIMEOUTMANAGER_H
