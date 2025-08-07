//
// Created by Var on 25-7-27.
//

#ifndef EPOLLER_H
#define EPOLLER_H
#pragma once

#include <vector>
#include <unordered_map>
#include <sys/epoll.h>
#include "Channel.h"

class Epoller {
public:
    Epoller();
    ~Epoller();

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    std::vector<Channel*> poll(int timesout);

private:
    int epoll_fd;
    std::unordered_map<int,Channel*> channels_;
    std::vector<struct epoll_event> events_;
};



#endif //EPOLLER_H
