//
// Created by Var on 25-7-25.
//

#ifndef CONNECTION_H
#define CONNECTION_H
#pragma once
#include <functional>
#include <memory>
#include <string>
#include "Channel.h"
#include "Socket.h"
#include "EventLoop.h"

class Connection : public std::enable_shared_from_this<Connection> {
public:
    using Ptr = std::shared_ptr<Connection>;
    using ActivityCallback = std::function<void(int)>;

    using MessageCallback = std::function<void(const Ptr&,const std::string&)>;
    using CloseCallback = std::function<void(const Ptr&)>;

    Connection(EventLoop * loop,int connfd);
    ~Connection();

    void setMessageCallback(MessageCallback cb);
    void setCloseCallback(CloseCallback cb);

    void send(const std::string& data);
    void shutdown();
    void enableRead();
    EventLoop* getLoop() const;
    void connectEstablished();

    void setActivityCallback(ActivityCallback cb);

    int fd() const;
private:
    enum class State {Connected, Disconnecting, Disconnected};

    void sendInLoop(const std::string& data);
    void shutdownInLoop();

    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();

    EventLoop * loop_;
    Socket socket_;
    Channel channel_;

    std::string inputBuffer_;
    std::string outputBuffer_;

    MessageCallback messageCallback_;
    CloseCallback closeCallback_;
    ActivityCallback updateActivityCallback_;

    State state_;
};



#endif //CONNECTION_H
