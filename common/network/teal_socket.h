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
    int  Listen(ipaddr_t ip, port_t port);

    void SetSequence(int seq) { m_sequence = seq; }
    void SetHandler(SocketHandler * handler) { m_handler = handler; }

    int  GetSequence() { return m_sequence; }
    int  GetSocketFd() { return m_socketFd; }
    int  GetState()    { return m_state; }

private:

    int m_proto;
    int m_sequence;
    int m_socketFd;
    int m_state;

    SocketHandler * m_handler;
};
#endif