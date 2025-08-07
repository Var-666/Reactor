//
// Created by Var on 25-8-7.
//

#include "mylib/Buffer.h"
#include <sys/uio.h>  //定义 struct iovec 和 readv
#include <cstring>

Buffer::Buffer(size_t initialSize):buffer_(initialSize),readerIndex_(0),writerIndex_(0) {
}

size_t Buffer::readableBytes() const {
    return writerIndex_ - readerIndex_;
}

size_t Buffer::writableBytes() const {
    return buffer_.size() - writerIndex_;
}

const char * Buffer::peek() const {
    return buffer_.data() + readerIndex_;
}

void Buffer::retrieve(size_t len) {
    if (len < readableBytes()) {
        readerIndex_ += len;
    }else {
        retrieveAll();
    }
}

void Buffer::retrieveAll() {
    readerIndex_ = writerIndex_ = 0;
}

std::string Buffer::retrieveAllAsString()  {
    std::string result(peek(), readableBytes());
    retrieveAll();
    return std::move(result);
}

void Buffer::append(const char *data, size_t len) {
    ensureWritableBytes(len);
    std::memcpy(buffer_.data() + writerIndex_, data, len);
    writerIndex_ += len;
}

void Buffer::append(const std::string &str) {
    append(str.c_str(), str.size());
}

void Buffer::ensureWritableBytes(size_t len) {
    if (writableBytes() < len) {
        makeSpace(len);
    }
}

ssize_t Buffer::readFd(int fd, int *savedErrno) {
    char extraBuf[65536];
    struct iovec vec[2];
    size_t writable = writableBytes();

    vec[0].iov_base = buffer_.data() + writerIndex_;
    vec[0].iov_len = writable;
    vec[1].iov_base = extraBuf;
    vec[1].iov_len = sizeof(extraBuf);

    const int iovcnt = (writable < sizeof(extraBuf)) ? 2 : 1;
    ssize_t n = readv(fd, vec, iovcnt);

    if (n < 0) {
        *savedErrno = errno;
    }else if (static_cast<size_t>(n) <= writable) {
        writerIndex_ += n;
    }else {
        writerIndex_ = buffer_.size();
        append(extraBuf,n - writable);
    }
    return n;
}

void Buffer::makeSpace(size_t size) {
    // 如果前面有足够空间可复用，移动数据
    if (readerIndex_ + writableBytes() >= size) {
        size_t readable = readableBytes();
        std::copy(buffer_.begin() + readerIndex_, buffer_.begin() + writerIndex_,buffer_.begin());
        readerIndex_ = 0;
        writerIndex_ = readable;
    }else {
        // 否则直接扩容
        buffer_.resize(writerIndex_ + size);
    }
}
