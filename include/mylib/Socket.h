//
// Created by Var on 25-7-25.
//

#ifndef SOCKET_H
#define SOCKET_H
#pragma once
#include <netinet/in.h>

class Socket {

public:
    Socket();
    explicit Socket(int fd);
    ~Socket();

    void bind(const sockaddr& addr) const;
    void listen() const;
    int accept(sockaddr* addr) const;
    int fd() const;
    void setNonBlocking(bool enable) const;
    void shutdownWrite() const;

private:
    int sockfd_;
};



#endif //SOCKET_H
