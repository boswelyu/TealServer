#include "teal_socket.h"
#include "logger.h"
#include <new>

int TealSocket::s_errno = 0;
const char * TealSocket::s_errorStr[] = 
{
    NULL,
    "No Enough Memory",
    "Bad Parameter",
};

TealSocket::TealSocket(int sendCacheSize, int recvCacheSize, int timeout)
{
    m_socketFd = -1;
    m_state = SOCKET_STATE_INVALID;

    m_sendCacheSize = sendCacheSize;
    m_recvCacheSize = recvCacheSize;
    m_timeout = timeout;

    m_sendDataSize = 0;
    m_recvDataOffset = m_sendCacheSize + 8;
    m_recvDataSize = 0;

    //在发送和接收缓冲的结束分别写入Guard值
    *((long long int*)(m_buffer + m_sendCacheSize)) = SocketGuardValue;   //goodGOOD的ASCII
    *((long long int*)(m_buffer + m_recvDataOffset + m_recvCacheSize)) = SocketGuardValue;
}

TealSocket::~TealSocket()
{
    if(m_socketFd > 0)
    {
        //TODO: Close Socket
    }
    m_socketFd = -1;
    m_state = SOCKET_STATE_INVALID;
}

bool TealSocket::Create(int type, int proto)
{
    m_socketFd = socket(AF_INET, type, proto);
    if(m_socketFd < 0)
    {
        return false;
    }

    m_state = SOCKET_STATE_INITED;

    //TODO: Init Send Recv Buffer etc.
    return true;
}

bool TealSocket::Create(int fd)
{
    if(fd < 0) return false;

    m_socketFd = fd;
    m_state = SOCKET_STATE_INITED;

    return true;
}

int TealSocket::Listen(ipaddr_t ip, port_t port)
{
    struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
    addr.sin_port = port;
	
    int ret = bind(m_socketFd, (struct sockaddr *)&addr, sizeof(addr));
    if( ret != 0)
    {
        return ret;
    }

    ret = listen(m_socketFd, SOCKET_BACKLOG);
    if(ret == 0)
    {
        m_state = SOCKET_STATE_LISTENING;
    }
    return ret;
}

bool TealSocket::GetSocketAddr(struct sockaddr * sa, int & salen) const
{
    struct sockaddr_in * addr = (struct sockaddr_in *)sa;
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = m_ip;
    addr->sin_port = m_port;

    salen = sizeof(struct sockaddr_in);
    return true;
}

TealSocket * TealSocket::Accept()
{
    struct sockaddr_in sa;
    socklen_t len = sizeof(sa);

    int nfd = accept(m_socketFd, (struct sockaddr *)&sa, &len);
    if(nfd < 0)
    {
        LOG_ERROR("Accept new socket Failed with error: %s", strerror(errno));
        return NULL;
    }

    if(m_handler->CanAccept((struct sockaddr *)&sa, len) == false)
    {
        LOG_ERROR("Accept from %s Rejected", AddrToStr((const struct sockaddr *)&sa, len));
        return NULL;
    }

    TealSocket * ns = TealSocket::Alloc(m_sendCacheSize, m_recvCacheSize, m_timeout);
    if(ns == NULL)
    {
        LOG_ERROR("Alloc new TealSocket Failed with error: %s", TealSocket::Error());
        return NULL;
    }

    if(ns->Create(nfd) == false)
    {
        LOG_ERROR("Teal Socket Create Failed with error: %s", TealSocket::Error());
        TealSocket::Free(ns);
        return NULL;
    }

    ipaddr_t remoteIp = *((ipaddr_t *)&sa.sin_addr);
    port_t   remotePort = sa.sin_port;

    ns->SetRemoteAddr(remoteIp, remotePort);
    ns->SetState(SOCKET_STATE_CONNECTED);

    ns->SetHandler(m_handler);

    return ns;
}

int TealSocket::Receive()
{
    return 0;
}

void TealSocket::SendCache()
{
    
}

/************ Static Function ***********/

TealSocket * TealSocket::Alloc(int sendCacheSize, int recvCacheSize, int timeout)
{
    //发送缓冲和接收缓冲的大小都8字节对齐，并在发送缓冲和接收缓冲的后面分别追加8字节的Guard数据
    int reminder = sendCacheSize % 8;
    if(reminder != 0)
    {
        sendCacheSize += (8 - reminder);
    }
    reminder = recvCacheSize % 8;
    if(reminder != 0)
    {
        recvCacheSize += (8 - reminder);
    }
    int extraSize = sendCacheSize + recvCacheSize + 16;
    int memsize = sizeof(TealSocket) - 1 + extraSize;
    void * memory = malloc(memsize);
    if(memory == NULL)
    {
        s_errno = ESOCK_NO_MEMORY;
        return NULL;
    }
    new (memory) TealSocket(sendCacheSize, recvCacheSize, timeout);
    return (TealSocket *)memory;
}

void TealSocket::Free(TealSocket * sock)
{
    delete sock;
}

char * TealSocket::AddrToStr(const struct sockaddr * sa, socklen_t salen)
{
    static char str[128];

    switch(sa->sa_family)
    {
    case AF_INET:
    {
        struct sockaddr_in * sin = (struct sockaddr_in *)sa;
        if(inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
        {
            LOG_ERROR("Convert addr to str failed with error: %s", strerror(errno));
            return NULL;
        }
        int addrlen = strlen(str);
        snprintf(str + addrlen - 1, sizeof(str) - addrlen, ":%d", ntohs(sin->sin_port));
    }
        return str;
    case AF_INET6:
    {
        struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)sa;
        if(inet_ntop(AF_INET6, &sin6->sin6_addr, str, sizeof(str)) == NULL)
        {
            LOG_ERROR("Convert IPv6 Address Failed with error: %s", strerror(errno));
            return NULL;
        }
        int addrlen = strlen(str);
        snprintf(str + addrlen - 1, sizeof(str) - addrlen, ":%d", ntohs(sin6->sin6_port));
    }
        return str;
    default:
        LOG_ERROR("Not Supported SA family: %d", sa->sa_family);
        break;
    }
    return NULL;
}

char * TealSocket::SocketToStr(const TealSocket * sock)
{
    if(sock == NULL) return NULL;

    int salen = 0;
    struct sockaddr sa;
    if(sock->GetSocketAddr(&sa, salen))
    {
        return AddrToStr(&sa, salen);
    }
    return NULL;
}