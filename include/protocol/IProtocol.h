//
// Created by Var on 25-8-9.
//

#ifndef IPROTOCOL_H
#define IPROTOCOL_H

#pragma once

#include <string>
#include "mylib/Buffer.h"

class IProtocol{
public:
    virtual ~IProtocol() = default;
    virtual bool decode(Buffer & buffer,std::string& msg) = 0;
    virtual std::string encode(const std::string& msg) = 0;
};

#endif //IPROTOCOL_H
