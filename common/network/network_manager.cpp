#include "network_manager.h"
#include "clock_counter.h"
#include "epoll_socket_listener.h"
#include "logger.h"


NetworkManager * NetworkManager::Create(int maxClient, ClockCounter * clockCounter)
{
    NetworkManager * nm = new NetworkManager();
    nm->SetClockCounter(clockCounter);
    ISocketListener * listener = new EpollSocketListener();

    if(!listener->Init(maxClient))
    {
        LOG_ERROR("Init EpollSocketListener with max client: %d Failed", maxClient);
        delete listener;
        delete nm;
        return NULL;
    }
    nm->SetSocketListener(listener);
    return nm;
}

NetworkManager::NetworkManager()
    : m_clockCounter(NULL)
{
    m_socketListener = NULL;
}

NetworkManager::~NetworkManager()
{

}

int NetworkManager::Listen(const char * ip, const char * port, SocketHandler * handler)
{
    ipaddr_t ipaddr = inet_addr(ip);
    port_t   ipport = htons((port_t)atol(port));

    TealSocket * sock = new TealSocket();
    sock->SetHandler(handler);

    if(sock->Listen(ip, port) == false)
    {
        LOG_ERROR("Listen on %s:%s Failed, Error: %s", ip, port, strerror(errno));
        delete sock;
        return -1;
    }

    int conn = sock->GetSocket();
    

    return 0;
}

void NetworkManager::Update()
{

}

void NetworkManager::Close(hconn_t conn)
{

}


