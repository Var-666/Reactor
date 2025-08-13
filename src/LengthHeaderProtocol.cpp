//
// Created by Var on 25-8-9.
//

#include "protocol/LengthHeaderProtocol.h"
#include <cstring>
#include <stdexcept>

LengthHeaderProtocol::LengthHeaderProtocol(size_t headerLen):headerLen_(headerLen) {
}

bool LengthHeaderProtocol::decode(Buffer &buffer, std::string &msg) {
    if (buffer.readableBytes() < headerLen_) {
        return false;
    }

    const auto data = buffer.peek();
    uint32_t bodyLen = 0;
    std::memcpy(&bodyLen, data, headerLen_);
    bodyLen = ntohl(bodyLen);

    // 限制最大包长，防止恶意攻击
    const uint32_t maxBodyLen = 64 * 1024 * 1024; // 64MB
    if (bodyLen > maxBodyLen) {
        // 直接丢弃或者触发错误处理
        throw std::runtime_error("Body length too large");
    }

    if (buffer.readableBytes() < headerLen_ + bodyLen) {
        return false;
    }

    buffer.retrieve(headerLen_);
    msg.assign(buffer.peek(), bodyLen);
    buffer.retrieve(bodyLen);

    return true;
}

std::string LengthHeaderProtocol::encode(const std::string &msg) {
    uint32_t len = htonl(static_cast<uint32_t>(msg.size()));
    std::string packet;
    packet.resize(headerLen_ + msg.size());
    std::memcpy(&packet[0],&len,headerLen_);
    std::memcpy(&packet[headerLen_],msg.data(),msg.size());
    return packet;
}
