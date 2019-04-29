#include "common.h"
#include "ftpproto.h"
#include "privparent.h"
#include "session.h"
#include "sysutil.h"

#include <arpa/inet.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <uuid/uuid.h>

void begin_session(session_t *s)
{
    // 将父进程变为nobody进程
    struct passwd *pw = getpwnam("nobody");
    if (pw == NULL)
    {
        return;
    }
    if (setegid(pw->pw_gid) < 0)
    {
        // 需要先修改组id再修改用户id
        ERR_EXIT("setgid");
    }
    if (seteuid(pw->pw_uid) < 0)
    {
        ERR_EXIT("setuid");
    }

    // 创建子线程
    pid_t pid = fork();
    if (pid < 0)
    {
        ERR_EXIT("fork");
    }

    // 子线程和主线程通讯 套接字对
    int sockfds[2];
    if (socketpair(PF_UNIX, SOCK_STREAM, 0, sockfds) < 0)
    {
        ERR_EXIT("socketpair");
    }

    if (pid == 0)
    {
        // 服务进程 FTP协议相关细节
        close(sockfds[0]);
        s->child_fd = sockfds[1];
        handle_child(s);
    }
    else
    {
        // Nobody进程
        close(sockfds[1]);
        s->parent_fd = sockfds[0];
        handle_parent(s);
    }
}
