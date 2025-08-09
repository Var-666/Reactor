//
// Created by Var on 25-8-9.
//

#ifndef LENGTHHEADERPROTOCOL_H
#define LENGTHHEADERPROTOCOL_H

#pragma once
#include "IProtocol.h"
#include "mylib/Buffer.h"
#include <arpa/inet.h>

class LengthHeaderProtocol : public IProtocol{
public:
  explicit LengthHeaderProtocol(size_t headerLen = 4);

  bool decode(Buffer& buffer,std::string& msg) override;
  std::string encode(const std::string& msg) override;
private:
  size_t headerLen_;

};



#endif //LENGTHHEADERPROTOCOL_H
