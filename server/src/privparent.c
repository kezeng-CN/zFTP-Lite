#include "privparent.h"
#include "sysutil.h"

#include <unistd.h>

void handle_parent(session_t *s)
{
    char cmd;
    while (1)
    {
        read(s->parent_fd, &cmd, 1);
    }
    // 解析内部命令
    // 处理内部命令
}
