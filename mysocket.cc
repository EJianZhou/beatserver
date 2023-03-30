#include "mysocket.h"
#include <iostream>

void my_log()
{
	std::cerr << std::endl;
}

void err_log(const std::string &msg)
{
	std::cerr << "Error: " << msg << " " << strerror(errno) << std::endl;
}

void err_quit(const std::string &msg)
{
	err_log(msg);
	exit(1);
}
int Socket(int family, int type, int protocol)
{
	int n = socket(family, type, protocol);
	if (n < 0)
	{
		err_quit("socket error");
	}
	return n;
}
void Bind(int fd, const sockaddr *sa, socklen_t salen)
{
	ssize_t n = bind(fd, sa, salen);
	if (n < 0)
	{
		err_quit("bind error");
	}
}
void Listen(int fd, int backlog)
{
	int n = listen(fd, backlog);
	if (n < 0)
	{
		err_quit("listen error");
	}
}
int Accept(int fd, sockaddr *sa, socklen_t *salenptr)
{
	// int n;
	// while (1) {
	// 	n = accept(fd, sa, salenptr);
	// 	if (n < 0) {

	// 		if (errno == EPROTO || errno == ECONNABORTED) continue;
	// 		else  {
	// 			err_log("accept error");
	// 			break;
	// 		}
	// 	} else break;
	// }
	// return n;
	int n = accept(fd, sa, salenptr);
	if (n < 0)
	{
		err_log("accept error");
	}
	return n;
}

int Inet_pton(int family, const char *strptr, void *addrptr)
{
	int n = inet_pton(family, strptr, addrptr);
	if (n != 1)
	{
		err_quit("inet_pton error");
	}
	return n;
}

bool Connect(int fd, const sockaddr *sa, socklen_t salen)
{
	int n = connect(fd, sa, salen);
	if (n != 0)
	{
		err_log("connect error");
	}
	return n == 0;
}

int Recv(int fd, char *buff, size_t nbytes, int flags)
{
	int n = recv(fd, buff, nbytes, flags);
	if (n == -1)
	{
		close(fd);
		err_log("recv error");
	}
	return n;
}
void Send(int fd, const char *buff, ssize_t nbytes, int flags)
{
	int n = send(fd, buff, nbytes, flags);
	if (n == -1)
	{
		err_log("send error");
	}
}
int Epoll_create(int size)
{
	int epfd = epoll_create(size);
	if (epfd == -1)
	{
		err_quit("epoll_create error");
	}
	return epfd;
}

void Epoll_ctl(int epfd, int op, int fd, epoll_event *event)
{
	int n = epoll_ctl(epfd, op, fd, event);
	if (n == -1)
	{
		err_log("epoll_ctl error");
	}
}

int Epoll_wait(int epfd, epoll_event *events, int maxevents, int timeout)
{
	int n = epoll_wait(epfd, events, maxevents, timeout);
	if (n == -1)
	{
		err_log("epoll_wait error");
	}
	return n;
}

void Setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
	int n = setsockopt(fd, level, optname, optval, optlen);
	if (n == -1)
	{
		err_quit("setsockopt error");
	}
}

void Close(int sockfd)
{
	if (close(sockfd) == -1)
	{
		err_log("close error");
	}
}

int setnonblocking(int sockfd)
{
	int flags;
	if ((flags = fcntl(sockfd, F_GETFL, 0)) == -1)
	{
		return -1;
	}
	if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		return -1;
	}
	return 0;
}

std::string get_ip_from_fd(int fd)
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	if (getpeername(fd, (struct sockaddr *)&addr, &len) == -1)
	{
		return "";
	}
	return inet_ntoa(addr.sin_addr);
}
