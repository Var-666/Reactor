//
// Created by Var on 25-8-9.
//

#include "mylib/Codec.h"
#include <iostream>

Codec::Codec(std::shared_ptr<IProtocol> protocol):protocol_(std::move(protocol)) {
}

bool Codec::decode(Buffer &buffer, std::vector<std::string> &msgs) {
    if (!protocol_) {
        return false;
    }

    bool decodeAny = false;
    std::string msg;

    while (protocol_->decode(buffer, msg)) {
        msgs.emplace_back(std::move(msg));
        decodeAny = true;
    }
    return decodeAny;
}

std::string Codec::encode(const std::string &msg) {
    if (!protocol_) {
        return {};
    }
    return protocol_->encode(msg);
}

void Codec::setProtocol(std::shared_ptr<IProtocol> protocol) {
    protocol_ = std::move(protocol);
}



