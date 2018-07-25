#include "network_manager.h"
#include "clock_counter.h"
#include "epoll_socket_listener.h"
#include "logger.h"

#define NETWORK_SEQUENCE_START 100

NetworkManager * NetworkManager::Create(int maxClient, ClockCounter * clockCounter)
{
    NetworkManager * nm = new NetworkManager();
    nm->SetClockCounter(clockCounter);
    ISocketListener * listener = new EpollSocketListener();

    if(!listener->Init(maxClient, nm))
    {
        LOG_ERROR("Init EpollSocketListener with max client: %d Failed", maxClient);
        delete listener;
        delete nm;
        return NULL;
    }
    nm->SetSocketListener(listener);

    return nm;
}

int NetworkManager::GetSequenceNum()
{
    return m_sequence++;
}

NetworkManager::NetworkManager()
    : m_clockCounter(NULL)
{
    m_socketListener = NULL;
    m_sequence = NETWORK_SEQUENCE_START;
}

NetworkManager::~NetworkManager()
{

}

int NetworkManager::Listen(const char * ipStr, const char * portStr, SocketHandler * handler)
{
    ipaddr_t ipaddr = inet_addr(ipStr);
    port_t   port = htons((port_t)atol(portStr));

    TealSocket * sock = new TealSocket();
    if(!sock->Create(SOCK_STREAM, IPPROTO_TCP))
    {
        LOG_ERROR("Create Socket Failed, Error: %s", strerror(errno));
        delete sock;
        return -1;
    }

    //Socket关闭后可能会被重复使用，使用递增的Sequence来保证数字不重复
    int seq = GetSequenceNum();
    sock->SetSequence(seq);
    sock->SetHandler(handler);

    int ret = sock->Listen(ipaddr, port);
    if(ret != 0)
    {
        LOG_ERROR("Listen on %s:%s Failed, Return: %d, Error: %s", ipStr, portStr, ret, strerror(errno));
        delete sock;
        return -1;
    }

    //在Listening socket上监听读事件
    m_socketListener->RegisterSocketEvent(sock, TealSocket::ListenEvent);
    m_socketSeqMap.insert(std::make_pair(seq, sock));

    return seq;
}

void NetworkManager::Update()
{
    m_socketListener->CheckSocketEvents(1);
}

void NetworkManager::HandleCloseEvent(TealSocket * socket)
{

}

void NetworkManager::HandleAcceptEvent(TealSocket * socket)
{

}

void NetworkManager::HandleReadEvent(TealSocket * socket)
{

}

void NetworkManager::HandleWriteEvent(TealSocket * socket)
{
    
}



