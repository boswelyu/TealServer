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

public:
    TealSocket();
    virtual ~TealSocket();

    bool Create(int type, int proto);
    bool Create(int fd);
    int  Listen(ipaddr_t ip, port_t port);
    TealSocket * Accept();        //接受新的连接
    int  Receive();               //读取收到的数据

    void SendCache();

    void SetSequence(int seq) { m_sequence = seq; }
    void SetHandler(SocketHandler * handler) { m_handler = handler; }
    void SetRemoteAddr(ipaddr_t ip, port_t port) { m_ip = ip; m_port = port; }
    void SetState(int state) { m_state = state; }

    int  GetSequence() { return m_sequence; }
    int  GetSocketFd() { return m_socketFd; }
    SocketHandler * GetHandler() { return m_handler; }
    int  GetState()    { return m_state; }
    bool GetSocketAddr(struct sockaddr * sa, int &len) const; 

public:
    static TealSocket * Alloc();
    static void Free(TealSocket * sock);

    static char * AddrToStr(const struct sockaddr * sa, socklen_t salen);
    static char * SocketToStr(const TealSocket * sock);
    static const char * Error() { return s_errorStr[s_errno]; }
private:
    static int s_errno;
    static const char * s_errorStr[ESOCK_MAX];

private:

    int m_proto;
    int m_sequence;
    int m_socketFd;
    int m_state;

    ipaddr_t m_ip;
    port_t   m_port;

    SocketHandler * m_handler;
};
#endif