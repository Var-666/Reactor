//
// Created by Var on 25-8-13.
//

#include "logger/LogFile.h"
#include <ctime>
#include <cassert>
#include <iostream>
#include <ostream>

LogFile::LogFile(const std::string &basename, size_t rollSize)
    :basename_(basename),
    rollSize_(rollSize),
    writtenBytes_(0),
    file_(nullptr,fclose){

    rollFile();
}

LogFile::~LogFile() {
    flush();
}

void LogFile::append(const char *logline, size_t len) {
    size_t written = fwrite(logline, 1, len, file_.get());
    writtenBytes_ += written;
    if (writtenBytes_ >= rollSize_) {
        rollFile();
    }
}

void LogFile::flush() const {
    fflush(file_.get());
}

bool LogFile::rollFile() {
    time_t now = time(nullptr);
    tm tm_time{};
    localtime_r(&now, &tm_time);

    char filename[256];
    snprintf(filename, sizeof(filename), "%s.%04d%02d%02d-%02d%02d%02d.log",
         basename_.c_str(),
         tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
         tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);

    std::cout << filename << std::endl;

    file_.reset(fopen(filename, "a"));
    writtenBytes_ = 0;
    if (!file_) {
        perror("LogFile::rollFile fopen failed");
    }
    return file_ != nullptr;
}
