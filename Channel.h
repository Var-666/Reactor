//
// Created by Var on 25-7-25.
//

#ifndef CHANNEL_H
#define CHANNEL_H
#pragma once
#include <functional>
#include <cstdint>
#include <iostream>
#include <memory>

// #include "EventLoop.h"

class EventLoop;

class Channel {

public:
    Channel(EventLoop * loop,int fd);
    ~Channel();

    void handleEvent();
    void handleEventWithGuard();
    void setReadCallback(std::function<void()> callback);
    void setWriteCallback(std::function<void()> callback);
    void setCloseCallback(std::function<void()> callback);
    void setErrorCallback(std::function<void()> callback);

    void tie(const std::shared_ptr<void>& obj);

    void enableRead();
    void enableWrite();
    void disableWrite();
    void disableAll();
    bool isWriting();

    int fd() const;
    uint32_t events() const;
    void setEvents(uint32_t revents);
private:
    int fd_;
    uint32_t events_;
    uint32_t revents_;

    std::weak_ptr<void> tie_;
    bool tied_ = false;

    EventLoop *loop_;

    std::function<void()> readCallback_;
    std::function<void()> writeCallback_;
    std::function<void()> closeCallback_;
    std::function<void()> errorCallback_;
};



#endif //CHANNEL_H
