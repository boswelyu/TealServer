#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdarg.h>
#include "iomn.h"

#define BUFF_SIZE 4096000

void * IomnEntry(void * param);

int g_conn_fd;
char *g_write_buff;
char* g_read_buff;
char* g_dump_buff;
int  g_dump_len = 0;
int g_buff_size = 0;

void (*g_start_func)();

pthread_t IomnStart(const char * socket_name, void (* start_func)())
{
    g_start_func = start_func;
    pthread_t tid;

    if(g_start_func == NULL)
    {
        printf("[ERROR] Invalid IOMN top menu function\n");
        return -1;
    }

    int ret = pthread_create(&tid, NULL, IomnEntry, (void *)socket_name);
    if(ret == 0) 
    {
        printf("==== IOMN Started Successful ====\n");
        return tid;
    }

    printf("++++ [ERROR] Start IOMN Thread Failed ++++\n");
    return -1;
}

void IomnExit(pthread_t tid)
{
    if(tid <= 0) return;

    pthread_cancel(tid);

	free(g_write_buff);
	free(g_read_buff);
	free(g_dump_buff);
}


void * IomnEntry(void * param)
{
    int pretval = 0;
    char * socket_name = (char *)param;
    int listen_fd = ListenOnLocalSocket(socket_name);
    if(listen_fd <= 0)
    {
        pretval = -1;
        pthread_exit(&pretval);
        return NULL;
    }

    // Listen for iomn client connect
    struct sockaddr_un clt_addr;
    socklen_t len = sizeof(clt_addr);

    g_write_buff = (char *)malloc(sizeof(char) * BUFF_SIZE);
    memset(g_write_buff, 0, BUFF_SIZE * sizeof(char));
	
	g_read_buff = (char*)malloc(sizeof(char)*BUFF_SIZE);
	memset(g_read_buff, 0, BUFF_SIZE*sizeof(char));

	g_dump_buff = (char*)malloc(sizeof(char)*BUFF_SIZE);
	memset(g_dump_buff, 0, BUFF_SIZE*sizeof(char));

	g_buff_size = BUFF_SIZE;

	while(1)
    {
        g_conn_fd = accept(listen_fd, (struct sockaddr*)&clt_addr, &len);
        if(g_conn_fd < 0)
        {
            perror("cannot accept client connect request");
            continue;
        }

        g_start_func();

        printf("client quit, wait for another client\n");
        close(g_conn_fd);
    }

    close(listen_fd);
    unlink(socket_name);

    pthread_exit(0);

    return NULL;
}

int ListenOnLocalSocket(char * socket_name)
{
    int listen_fd, ret;
    struct sockaddr_un svr_addr;

    listen_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if(listen_fd < 0)
    {
        printf("[ERROR] Cannot create local communication socket for iomn_zonesvr");
        return -1;
    }

    // Set Local socket info
    svr_addr.sun_family = AF_UNIX;
    strncpy(svr_addr.sun_path, socket_name, sizeof(svr_addr.sun_path)-1);
    svr_addr.sun_path[0] = 0;
    socklen_t svr_len = strlen(socket_name) + offsetof(struct sockaddr_un, sun_path);
    unlink(socket_name);

    // Bind to local socket
    ret = bind(listen_fd, (struct sockaddr*)&svr_addr, svr_len);
    if(ret == -1)
    {
        printf("[ERROR] Cannot bind local server socket for iomn_zonesvr");
        close(listen_fd);
        unlink(socket_name);
        return -1;
    }
    //listen sockfd
    ret = listen(listen_fd,1);
    if(ret == -1)
    {
        printf("[ERROR] Cannot listen the iomn client connect request");
        close(listen_fd);
        unlink(socket_name);
        return -1;
    }

    return listen_fd;
}

// wrapper function of gets, printf etc
char * iomn_gets(char * buffer, int len)
{
    int n = read(g_conn_fd, buffer, len);
    if(n == 0) return NULL;

    buffer[n] = 0;
    return buffer;
}

void iomn_push(const char * format, ...)
{
    int left = BUFF_SIZE - g_dump_len;
    va_list _valist;
    va_start(_valist, format);
    int len = vsnprintf(g_write_buff + g_dump_len, left, format, _valist);
    va_end(_valist);

    if(len >= left - 1024) 
    {
        // 快满了，写出
        write(g_conn_fd, g_write_buff, g_dump_len + len);
        g_dump_len = 0;
    }
    else
    {
        g_dump_len += len;
    }
}

// always flush after formated
void iomn_print(const char * format, ...)
{
    int left = BUFF_SIZE - g_dump_len;
    if(left < 1024) {
        iomn_flush();
    }
    va_list _valist;
    va_start(_valist, format);
    int len = vsnprintf(g_write_buff + g_dump_len, left, format, _valist);
    va_end(_valist);

    g_dump_len += len;

    iomn_flush();
}

void iomn_flush()
{
    if(g_dump_len <= 0) return;

    write(g_conn_fd, g_write_buff, g_dump_len);
    g_dump_len = 0;
}

void iomn_print(int len, const char * buff)
{
    write(g_conn_fd, buff, len);
}
