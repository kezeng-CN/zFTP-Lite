#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"
#include "session.h"
#include "sysutil.h"

int main(void)
{
    if (getuid() != 0)
    {
        fprintf(stderr, "zFTP-lite must be started as root\n");
    }

    int listenfd = tcp_server(NULL, 5000);
    pid_t pid;
    int conn;
    session_t sess = {-1, "", "", "", -1, -1};

    while (1)
    {
        conn = accept_timeout(listenfd, NULL, 0);
        if (conn == -1)
        {
            ERR_EXIT("accept_timeout");
        }

        // 创建子进程处理
        pid = fork();
        if (pid == -1)
        {
            ERR_EXIT("fork");
        }

        if (pid == 0)
        {
            close(listenfd);
            sess.ctrl_fd = conn;
            begin_session(&sess);
        }
        else
        {
            close(conn);
        }
    }

    return 0;
}
