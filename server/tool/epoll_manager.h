#pragma once

#include <sys/epoll.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>


class epoll_manager {
public:
    epoll_manager() {
        epollfd_ = epoll_create(1);
        if (epollfd_ == -1) {
            perror("epoll_create error");
            exit(EXIT_FAILURE);
        }
    }

    ~epoll_manager() {
        close(epollfd_);
    }

    void add(int fd, uint32_t events, void* ptr = nullptr) {
        epoll_event ev;
        ev.data.fd = fd;
        if (ptr) ev.data.ptr = ptr;
        ev.events = events;
        if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev) == -1) {
            perror("epoll_ctl error");
            exit(EXIT_FAILURE);
        }
    }

    void modify(int fd, uint32_t events, void* ptr = nullptr) {
        epoll_event ev;
        ev.data.fd = fd;
        if (ptr) ev.data.ptr = ptr;
        ev.events = events;
        if (epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &ev) == -1) {
            perror("epoll_ctl error");
            exit(EXIT_FAILURE);
        }
    }

    void del(int fd) {
        epoll_event ev;
        if (epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &ev) == -1) {
            perror("epoll_ctl error");
            exit(EXIT_FAILURE);
        }
    }

    int wait(epoll_event* events, int maxevents, int timeout) {
        int nfds = epoll_wait(epollfd_, events, maxevents, timeout);
        if (nfds == -1) {
            perror("epoll_wait error");
            exit(EXIT_FAILURE);
        }
        return nfds;
    }

private:
    int epollfd_;
};

