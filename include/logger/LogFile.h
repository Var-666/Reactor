//
// Created by Var on 25-8-13.
//

#ifndef LOGFILE_H
#define LOGFILE_H

#include <string>
#include <memory>
#include <cstdio>

class LogFile {
public:
    LogFile(const std::string& basename, size_t rollSize);
    ~LogFile();

    void append(const char* logline, size_t len);
    void flush() const;
private:
    bool rollFile();

    std::string basename_;
    size_t rollSize_;
    size_t writtenBytes_;
    std::unique_ptr<FILE, decltype(&fclose)> file_;
};



#endif //LOGFILE_H
