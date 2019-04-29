#ifndef _SYS_UTIL_H_
#define _SYS_UTIL_H_

#include <arpa/inet.h>

int getlocalip(char *ip);

void activate_nonblock(int fd);
void deactivate_nonblock(int fd);

int read_timeout(int fd, unsigned int wait_seconds);
int write_timeout(int fd, unsigned int wait_seconds);

int accept_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds);
int connect_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds);

long readn(int fd, void *buf, unsigned int count);
long writen(int fd, const void *bug, unsigned int count);

long recv_peek(int sockfd, void *buf, unsigned int len);
long readline(int sockfd, void *buf, unsigned int maxline);

void send_fd(int sock_fd, int fd);
int recv_fd(const int sock_fd);

int tcp_server(const char *host, unsigned short port);

#endif
