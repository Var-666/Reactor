//
// Created by Var on 25-7-25.
//

#include "mylib/InetAddress.h"
#include <arpa/inet.h>
#include <cstring>  // 为了 memset


InetAddress::InetAddress() {
  memset(&addr_, 0, sizeof(addr_));
}

InetAddress::InetAddress(uint16_t port) {
  memset(&addr_, 0, sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_port = htons(port);
  addr_.sin_addr.s_addr = INADDR_ANY;
}

InetAddress::InetAddress(std::string ip,uint16_t port) {
  memset(&addr_, 0, sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_port = htons(port);
  if (inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr) <= 0) {
    throw std::invalid_argument("Invalid IP address: " + ip);
  }
}

InetAddress::InetAddress(sockaddr_in addr):addr_(addr) {
}

std::string InetAddress::getIP() const {
  char buf[INET_ADDRSTRLEN];
  const char* result = inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
  if (result == nullptr) {
    return "";
  }
  return std::string(buf);
}

uint16_t InetAddress::getPort() const {
  return ntohs(addr_.sin_port);
}

// sockaddr_in & InetAddress::getSockAddr(){
//   return addr_;
// }

const sockaddr *InetAddress::getSockAddr() const {
  return reinterpret_cast<const sockaddr *>(&addr_);
}

void InetAddress::setSockAddr(const sockaddr_in& addr) {
  addr_ = addr;
}





