#ifndef _TEAL_SOCKET_H_
#define _TEAL_SOCKET_H_

#include "common.h"
#include "socket_handler.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define SOCKET_BACKLOG 1024

typedef unsigned long ipaddr_t;
typedef unsigned short port_t;

enum TealSocketEvent
{
    SOCKET_EVENT_ON_READ = 1,
    SOCKET_EVENT_ON_WRITE = 2,
};

enum TealSocketState
{
    SOCKET_STATE_INVALID    = 0,
    SOCKET_STATE_INITED     = 1,
    SOCKET_STATE_LISTENING  = 2,
    SOCKET_STATE_CONNECTING = 3,
    SOCKET_STATE_CONNECTED  = 4,
    SOCKET_STATE_CLOSING    = 5,
    SOCKET_STATE_CLOSED     = 6,
};

enum SocketCloseReason
{
    SOCKET_CR_READ_FAIL     = 1,  
    SOCKET_CR_REMOTE_CLOSE  = 2,
    SOCKET_CR_CLOSE_EVENT   = 3,
    SOCKET_CR_INVALID_PKG   = 4,
};

enum SocketErrorNo
{
    ESOCK_NONE      = 0,
    ESOCK_NO_MEMORY = 1,
    ESOCK_BAD_PARAM = 2,
    ESOCK_MAX
};

class TealSocket
{
public:
    static const int ListenEvent = SOCKET_EVENT_ON_READ;
    static const int ReadEvent = SOCKET_EVENT_ON_READ;
    static const int WriteEvent = SOCKET_EVENT_ON_WRITE;
    static const int ReadWriteEvent = SOCKET_EVENT_ON_READ | SOCKET_EVENT_ON_WRITE;
    static const long long int SocketGuardValue = 0x5357454e444f4f47L;   //GOODNEWS的ASCII

public:
    TealSocket(int sendCacheSize, int recvCacheSize, int timeout);
    virtual ~TealSocket();

    bool Create(int type, int proto);
    bool Create(int fd);
    int  Listen(ipaddr_t ip, port_t port);
    TealSocket * Accept();        //接受新的连接
    int  Receive();               //读取收到的数据
    int  SendCache();
    void Close();

    void SetConnMark(int seq) { m_marker = seq; }
    void SetHandler(SocketHandler * handler) { m_handler = handler; }
    void SetRemoteAddr(ipaddr_t ip, port_t port) { m_ip = ip; m_port = port; }
    void SetState(int state) { m_state = state; }

    int  GetConnMark() { return m_marker; }
    int  GetSocketFd() { return m_socketFd; }
    SocketHandler * GetHandler() { return m_handler; }
    int  GetState()    { return m_state; }
    bool GetSocketAddr(struct sockaddr * sa, int &len) const; 

    /* 当前缓冲区操作函数 */

    //当前接收缓冲区内容长度
    int RecvDataBytes()             { return m_recvDataSize; }           

    //当前接收缓冲区内容首地址
    const char * GetRecvDataPtr()   { return m_buffer + m_recvDataOffset; } 
    
    //从接收缓冲区中移除指定长度的内容
    void RemoveRecvData(int len);  

public:
    static TealSocket * Alloc(int sendCacheSize, int recvCacheSize, int timeout);
    static void Free(TealSocket * sock);

    static char * AddrToStr(const struct sockaddr * sa, socklen_t salen);
    static char * SocketToStr(const TealSocket * sock);
    static const char * Error() { return s_errorStr[s_errno]; }

private:
    void UpdateRecvTime();

private:
    static int s_errno;
    static const char * s_errorStr[ESOCK_MAX];

private:
 
    int m_marker;                     //数量递增的内部连接标记
    int m_socketFd;
    int m_state;
    time_t m_activeTime;              //最近一次活动时间

    ipaddr_t m_ip;
    port_t   m_port;
    short    m_timeout;     
    SocketHandler * m_handler;

    int m_sendCacheSize;
    int m_sendDataSize;

    int m_recvCacheSize;
    int m_recvCacheStart;
    int m_recvDataOffset;
    int m_recvDataSize;
 
    //后续连续的m_sendCacheSize + m_readCacheSize + 两块8字节的Guard Value 字节用作发送和接收缓冲区，发送在前，接收在后
    char m_buffer[0];
};
#endif