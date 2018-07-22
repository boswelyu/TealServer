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

hconn_t NetworkManager::Listen(const char * ip, const char * port, SocketHandler * handler)
{
    return 0;
}

void NetworkManager::Update()
{

}

void NetworkManager::Close(hconn_t conn)
{

}


