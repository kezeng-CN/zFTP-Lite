#include "ftpproto.h"
#include "string.h"
#include "sysutil.h"

void handle_child(session_t *s)
{
    writen(s->ctrl_fd, "220 (zFTP-lite 0.1)\r\n", strlen("220 (zFTP-lite 0.1)\r\n"));
    while (1)
    {
        memset(s->cmdline, 0, sizeof(s->cmdline));
        memset(s->cmd, 0, sizeof(s->cmd));
        memset(s->arg, 0, sizeof(s->arg));
        readline(s->ctrl_fd, s->cmdline, MAX_COMMAND_LINE);

        // 解析FTP命令与参数
        // 处理FTP命令
    }
}
