//
// Created by Var on 25-7-25.
//

#include "mylib/Socket.h"

#include <stdexcept>
#include <unistd.h>
#include<arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <cstring>


//创建 socket，设置 SO_REUSEADDR
Socket::Socket() {
    sockfd_ = socket(AF_INET,SOCK_STREAM,0);
    if (sockfd_ == -1) {
        throw std::runtime_error("Socket creation failed");
    }
    int opt = 1;
    setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setNonBlocking(true); //设置为非阻塞
}

Socket::Socket(int fd):sockfd_(fd) {
}

Socket::~Socket() {
    if (sockfd_ != -1) {
        sockfd_ = -1;
        close(sockfd_);
    }
}

//绑定到 IP 和端口
void Socket::bind(const sockaddr& addr) const{
    if (::bind(sockfd_,&addr,sizeof(addr)) == -1) {
        throw std::runtime_error(std::string("Bind failed: "));
    }
}

//启动监听
void Socket::listen() const {
    if (::listen(sockfd_,10) == -1) {
        throw std::runtime_error("Listen failed");
    }
    std::cout << "Listening on socket " << sockfd_ << std::endl;
}

//接收连接，返回新的连接 fd
int Socket::accept(sockaddr* addr) const {
    socklen_t len = sizeof(*addr);
    int client_fd = ::accept(sockfd_,addr,&len);
    if (client_fd == -1) {
        throw std::runtime_error("Accept failed");
    }
    // ✅ 设置 client_fd 非阻塞
    int flags = fcntl(client_fd, F_GETFL, 0);
    fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
    return client_fd;
}

//获取原始 socket fd
int Socket::fd() const {
    return sockfd_;
}

void Socket::setNonBlocking(const bool enable) const {
    int flags = fcntl(sockfd_, F_GETFL, 0);
    if (flags == -1) {
        throw std::runtime_error("fcntl failed");
    }
    if (enable) {
        flags |= O_NONBLOCK;
    }else {
        flags &= ~O_NONBLOCK;
    }
    if (fcntl(sockfd_, F_SETFL, flags) == -1) {
        throw std::runtime_error("fcntl failed");
    }
}

void Socket::shutdownWrite() const {
    if (::shutdown(sockfd_, SHUT_WR) == -1) {
        perror("Socket shutdownWrite error");
    }
}


