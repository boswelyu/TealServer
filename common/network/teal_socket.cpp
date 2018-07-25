#include "teal_socket.h"

TealSocket::TealSocket()
{
    m_socketFd = -1;
    m_state = SOCKET_STATE_INVALID;
}

TealSocket::~TealSocket()
{

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
