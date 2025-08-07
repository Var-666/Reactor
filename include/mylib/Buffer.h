//
// Created by Var on 25-8-7.
//

#ifndef BUFFER_H
#define BUFFER_H

#pragma once

#include <string>
#include <vector>

//•	管理读写缓冲区，避免直接操作 std::string 带来的性能问题；
//•	支持：
//•	从 socket 中读入数据；
//•	向 socket 写出数据；
//•	保留未处理的数据（用于拆包）；
//•	高效 prepend、append、retrieve 操作；

class Buffer {
public:
  explicit Buffer(size_t initialSize = 1024);
  size_t readableBytes() const;
  size_t writableBytes() const;

  const char* peek() const;
  void retrieve(size_t len);
  void retrieveAll();
  std::string retrieveAllAsString();

  void append(const char* data, size_t len);
  void append(const std::string& str);
  void ensureWritableBytes(size_t len);

  ssize_t readFd(int fd,int* savedErrno);

private:
  void makeSpace(size_t size);

  std::vector<char> buffer_;
  size_t readerIndex_;
  size_t writerIndex_;
};



#endif //BUFFER_H
