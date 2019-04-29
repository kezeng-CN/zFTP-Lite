#ifndef SESSION_H
#define SESSION_H

#include "common.h"

typedef struct session
{
    // 控制连接
    int ctrl_fd;
    char cmdline[MAX_COMMAND_LINE];
    char cmd[MAX_COMMAND];
    char arg[MAX_ARG];
    // 父子进程间通道
    int parent_fd;
    int child_fd;
} session_t;

void begin_session(session_t *sess);

#endif
