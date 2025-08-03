//
// Created by Var on 25-7-27.
//

#include "Epoller.h"
#include "Channel.h"
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <sys/epoll.h>


Epoller::Epoller():epoll_fd(::epoll_create1(EPOLL_CLOEXEC)),events_(1024) {
    if (epoll_fd < 0) {
        std::cerr << "epoll_create1 error" << std::endl;
        exit(1);
    }
}

Epoller::~Epoller() {
    close(epoll_fd);
}

void Epoller::updateChannel(Channel *channel) {
    epoll_event ev {};
    ev.events = channel->events();
    ev.data.ptr = channel;

    int fd = channel->fd();

    if (channel->events() == 0) {
        removeChannel(channel);
        return;
    }

    if (channels_.count(fd) == 0) {
        if (epoll_ctl(epoll_fd,EPOLL_CTL_ADD,fd,&ev) == -1) {
            perror("epoll_ctl ADD");
        }
        channels_[fd] = channel;
    }else {
        if (epoll_ctl(epoll_fd,EPOLL_CTL_MOD,fd,&ev) == -1) {
            perror("epoll_ctl MOD");
        }
    }
}

void Epoller::removeChannel(Channel *channel) {
    int fd = channel->fd();
    if (epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,nullptr) == -1) {
        perror("epoll_ctl DEL");
    }
    channels_.erase(fd);
}

std::vector<Channel *> Epoller::poll(int timesout) {
    int n = epoll_wait(epoll_fd,events_.data(),static_cast<int>(events_.size()),timesout);
    std::vector<Channel *> activeChannels;

    if (n < 0) {
        if (errno != EINTR) {
            std::cerr << "epoll_wait error: "  << std::endl;
        }
    }

    for (int i = 0; i < n; ++i) {
        Channel* ch = static_cast<Channel *>(events_[i].data.ptr);
        ch->setEvents(events_[i].events);
        activeChannels.push_back(ch);
    }

    return activeChannels;
}
