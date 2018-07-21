#include "os_base.h"

//后台方式启动服务
void StartDaemon()
{
    pid_t pid;

    if ((pid = fork()) != 0) exit(0);

    setsid();

    if ((pid = fork()) != 0) exit(0);

    umask(0); 
}