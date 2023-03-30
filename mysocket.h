#pragma once
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <string>
#include <string.h>
#include <unordered_set>
#include <iostream>

void my_log();

template <typename T, typename... Args>
void my_log(T x, Args... args)
{
    std::cerr << x << " ";
    my_log(args...);
}

void err_log(const std::string &);
void err_quit(const std::string &);

int Socket(int, int, int);
void Bind(int, const sockaddr *, socklen_t);
void Listen(int, int);
int Accept(int, sockaddr *, socklen_t *);

int Inet_pton(int, const char *, void *);
bool Connect(int, const sockaddr *, socklen_t);

int Recv(int, char *, size_t, int);
void Send(int, const char *, ssize_t, int);

int Epoll_create(int);
void Epoll_ctl(int, int, int, epoll_event *);
int Epoll_wait(int, epoll_event *, int, int);

void Close(int);

void Setsockopt(int __fd, int __level, int __optname, const void *__optval, socklen_t __optlen);

int setnonblocking(int);

std::string get_ip_from_fd(int fd);
