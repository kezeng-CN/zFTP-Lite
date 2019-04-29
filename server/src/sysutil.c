#include "common.h"
#include "sysutil.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h> //获取本地IP
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> //获取主机名

int getlocalip(char *ip)
{
    char host[100] = {0};
    if (gethostname(host, sizeof(host)) < 0)
        return -1;
    struct hostent *hp = gethostbyname(host);
    if (hp == NULL)
        return -1;
    strcpy(ip, inet_ntoa(*(struct in_addr *)hp->h_addr_list[0]));
    return 0;
}

// 设置I/O为非阻塞模式
void activate_nonblock(int fd)
{
    int ret;
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1)
        ERR_EXIT("fcntl");

    flags |= O_NONBLOCK;
    ret = fcntl(fd, F_SETFL, flags);
    if (ret == -1)
        ERR_EXIT("fcntl");
}
// 设置I/O为阻塞模式
void deactivate_nonblock(int fd)
{
    int ret;
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1)
        ERR_EXIT("fcntl");

    flags &= ~O_NONBLOCK;
    ret = fcntl(fd, F_SETFL, flags);
    if (ret == -1)
        ERR_EXIT("fcntl");
}
// 超时检测函数
int read_timeout(int fd, unsigned int wait_seconds)
{
    int ret = 0;
    if (wait_seconds > 0)
    {
        fd_set read_fdset; // 文件描述符集合，long类型数组
        struct timeval timeout;

        FD_ZERO(&read_fdset);    // 清空集合
        FD_SET(fd, &read_fdset); // 将fd加入集合
        // FD_CLR(int, fd_set *)    // 删除集合
        // FD_ISSET(int, fd_set *) //检查集合中指定的文件描述符是否可以读写

        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;

        do
        {
            ret = select(fd + 1, &read_fdset, NULL, NULL, &timeout);
        } while (ret < 0 && errno == EINTR);

        if (ret == 0)
        {
            ret = -1;
            errno = ETIMEDOUT;
        }
        else if (ret == 1)
        {
            ret = 0;
        }
    }
    return ret;
}

int write_timeout(int fd, unsigned int wait_seconds)
{
    int ret = 0;
    if (wait_seconds > 0)
    {
        fd_set write_fdset; // 文件描述符集合，long类型数组
        struct timeval timeout = {wait_seconds, 0};

        FD_ZERO(&write_fdset);
        FD_SET(fd, &write_fdset);

        do
        {
            ret = select(fd + 1, &write_fdset, NULL, NULL, &timeout);
        } while (ret < 0 && errno == EINTR);

        if (ret == 0)
        {
            ret = -1;
            errno = ETIMEDOUT;
        }
        else if (ret == 1)
        {
            ret = 0;
        }
    }
    return ret;
}

int accept_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds)
{
    int ret = 0;
    socklen_t addrlen = sizeof(struct sockaddr_in);

    if (wait_seconds > 0)
    {
        fd_set accept_fdset;
        FD_ZERO(&accept_fdset);
        FD_SET(fd, &accept_fdset);

        struct timeval timeout = {wait_seconds, 0};

        do
        {
            ret = select(fd + 1, &accept_fdset, NULL, NULL, &timeout);
        } while (ret < 0 && errno == EINTR);

        if (ret == -1)
        {
            return -1;
        }
        else if (ret == 0)
        {
            errno = ETIMEDOUT;
            return -1;
        }
    }
    if (addr != NULL)
    {
        ret = accept(fd, (struct sockaddr *)addr, &addrlen);
    }
    else
    {
        ret = accept(fd, NULL, NULL);
    }
    return ret;
}

int connect_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds)
{
    int ret = 0;
    return ret;
}

long readn(int fd, void *buf, unsigned int count)
{
    int ret = 0;
    return ret;
}

long writen(int fd, const void *bug, unsigned int count)
{
    int ret = 0;
    return ret;
}

long recv_peek(int sockfd, void *buf, unsigned int len)
{
    int ret = 0;
    return ret;
}

long readline(int sockfd, void *buf, unsigned int maxline)
{
    int ret = 0;
    return ret;
}

void send_fd(int sock_fd, int fd)
{
}

int recv_fd(const int sock_fd)
{
    int ret = 0;
    return ret;
}

// 用来启动tcp服务器
// @host：服务器IP地址/服务器主机名
// @port：服务器端口号
// 成功返回监听套接字
int tcp_server(const char *host, unsigned short port)
{
    // Creat socket
    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
    {
        ERR_EXIT("tcp_server");
    }

    // Get sockaddr_in
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if (host == NULL)
    {
        // 如果host是空，可能是任意指针
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        // 先判断是否为IP，再判断是否为主机名
        if (inet_aton(host, &servaddr.sin_addr) == 0)
        {
            struct hostent *hp;
            if ((hp = gethostbyname(host)) == NULL)
            {
                ERR_EXIT("host is invalid");
            }
            else
            {
                servaddr.sin_addr = *(struct in_addr *)hp->h_addr_list[0];
            }
        }
    }

    // Bind
    int on = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                   (const char *)&on, sizeof(on)) < 0)
    {
        ERR_EXIT("gethostbyname");
    }
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr)) < 0)
    {
        ERR_EXIT("bind");
    }

    // listen
    if (listen(listenfd, 0) < 0)
    {
        ERR_EXIT("listen");
    }

    return listenfd;
}
