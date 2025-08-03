//
// Created by Var on 25-7-27.
//

#include "Acceptor.h"

#include <unistd.h>


Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr)
    :loop_(loop),
    listenSocket_(),
    listenChannel_(loop,listenSocket_.fd()),
    listening_(false)
{
    listenSocket_.bind(*listenAddr.getSockAddr());

    listenChannel_.setReadCallback([this]() {
        this->handleRead();
    });
}

Acceptor::~Acceptor() {
    listenChannel_.disableAll();
    listenChannel_.~Channel();
}

void Acceptor::setNewConnectionCallback(const NewConnectionCallback &cb) {
    newConnectionCallback_ = std::move(cb);
}

void Acceptor::listen() {
    listening_ = true;
    listenSocket_.listen();
    listenChannel_.enableRead();
}

bool Acceptor::isListening() const {
    return listening_;
}

void Acceptor::handleRead() const {
    InetAddress clientAddr;
    int connfd = listenSocket_.accept(reinterpret_cast<sockaddr *>(&clientAddr));

    if (connfd >= 0) {
        if (newConnectionCallback_) {
            newConnectionCallback_(connfd, clientAddr);
        }else {
            close(connfd);
        }
    }else {
        perror("Acceptor::handleRead()");
    }

}
