//
// Created by Var on 25-7-25.
//

#ifndef INETADDRESS_H
#define INETADDRESS_H
#pragma once
#include <string>
#include <iostream>
#include <netinet/in.h>


class InetAddress {
public:
    InetAddress();
    explicit InetAddress(uint16_t port);
    InetAddress(std::string ip,uint16_t port);
    explicit InetAddress(sockaddr_in addr);

    std::string getIP() const;
    uint16_t getPort() const;

    // sockaddr_in &getSockAddr();
    const sockaddr *getSockAddr() const;
    void setSockAddr(const sockaddr_in& addr);
private:
    sockaddr_in addr_{};
};



#endif //INETADDRESS_H
