//
// Created by Var on 25-8-9.
//

#ifndef CODEC_H
#define CODEC_H

#pragma once

#include <string>
#include <memory>
#include <vector>
#include "mylib/buffer.h"
#include "protocol/IProtocol.h"

class Codec {
public:
  explicit Codec(std::shared_ptr<IProtocol> protocol);

  bool decode(Buffer& buffer, std::vector<std::string>& msgs);
  std::string encode(const std::string &msg);

  void setProtocol(std::shared_ptr<IProtocol> protocol);

private:
  std::shared_ptr<IProtocol> protocol_;
};



#endif //CODEC_H
