//
// Created by Var on 25-8-3.
//

#include "mylib/ConnectionTimeoutManager.h"

ConnectionTimeoutManager::ConnectionTimeoutManager(EventLoop *loop, int timeoutSecond)
    : loop_(loop),
    timeoutSeconds_(timeoutSecond){
    loop_->runEvery(1.0,[this](){checkTimeout();});
}

ConnectionTimeoutManager::~ConnectionTimeoutManager() {
}

void ConnectionTimeoutManager::addConnection(std::shared_ptr<Connection> conn) {
    int fd = conn->fd();
    connections_[fd] = connInfo{
        .conn = conn,
        .lastActive = std::chrono::steady_clock::now()
    };
}

void ConnectionTimeoutManager::updateConnectionActivity(int connfd) {
    auto it = connections_.find(connfd);
    if (it != connections_.end()) {
        it->second.lastActive = std::chrono::steady_clock::now();
    }
}

void ConnectionTimeoutManager::removeConnection(int connfd) {
    connections_.erase(connfd);
}

void ConnectionTimeoutManager::checkTimeout() {
    auto now = std::chrono::steady_clock::now();

    for (auto it = connections_.begin(); it != connections_.end();) {
        auto& info = it->second;
        auto conn = info.conn.lock();
        if (!conn) {
            it = connections_.erase(it);
            continue;
        }

        double idleTime = duration_cast<std::chrono::seconds>(now - info.lastActive).count();
        if (idleTime >= timeoutSeconds_) {
            std::cout << "Connection fd=" << conn->fd() << " timeout, closing\n";
            conn->shutdown(); // 关闭连接
            it = connections_.erase(it); // 移除
        } else {
            ++it;
        }
    }
}
