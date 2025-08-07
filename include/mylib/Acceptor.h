//
// Created by Var on 25-7-27.
//

#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#pragma once

#include <functional>
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"
#include "InetAddress.h"

class Acceptor {
public:
  using NewConnectionCallback = std::function<void(int sockfd,const InetAddress &peerAddr)>;

  Acceptor(EventLoop *loop, const InetAddress &listenAddr);
  ~Acceptor();

  void setNewConnectionCallback(const NewConnectionCallback& cb);

  void listen();

  bool isListening() const;
private:
  void handleRead() const;

  EventLoop *loop_;
  Socket listenSocket_;
  Channel listenChannel_;
  bool listening_;

  NewConnectionCallback newConnectionCallback_;
};



#endif //ACCEPTOR_H
