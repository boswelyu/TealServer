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

    m_closeSockets.clear();
}

NetworkManager::~NetworkManager()
{
    m_closeSockets.clear();
}

int NetworkManager::Listen(const char * ipStr, const char * portStr, SocketHandler * handler, 
    int sendCacheSize, int recvCacheSize, int timeout)
{
    ipaddr_t ipaddr = inet_addr(ipStr);
    port_t   port = htons((port_t)atol(portStr));

    TealSocket * sock = TealSocket::Alloc(sendCacheSize, recvCacheSize, timeout);
    if(!sock->Create(SOCK_STREAM, IPPROTO_TCP))
    {
        LOG_ERROR("Create Socket Failed, Error: %s", strerror(errno));
        TealSocket::Free(sock);
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
        TealSocket::Free(sock);
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

    ProcessSocketSend();

    ProcessSocketClose();

    CheckSocketTimeout();
}

void NetworkManager::HandleAcceptEvent(TealSocket * socket)
{
    TealSocket * ns = socket->Accept();
    if(ns == NULL)
    {
        LOG_ERROR("Accept Failed");
        return;
    }

    ns->SetSequence(GetSequenceNum());

    if(AppendNewSocket(ns) == false)
    {
        TealSocket::Free(ns);
        return;
    }

    //新连接建立成功，通知连接建立事件
    ns->GetHandler()->OnAccept(this, socket, ns);
}

void NetworkManager::HandleReadEvent(TealSocket * socket)
{
    if(socket->GetState() != SOCKET_STATE_CONNECTED)
    {
        LOG_ERROR("Internal Error, Socket %s which have read event but not in Connected state", TealSocket::SocketToStr(socket));
        return;
    }

    int readLen = socket->Receive();
    if(readLen < 0)
    {
        CloseSocket(socket, SOCKET_CR_READ_FAIL);
    }
}

void NetworkManager::HandleWriteEvent(TealSocket * socket)
{
    
}

void NetworkManager::HandleCloseEvent(TealSocket * socket)
{
    if(socket == NULL) return;

    int state = socket->GetState();
    if(state == SOCKET_STATE_CLOSING || state == SOCKET_STATE_CLOSED)
    {
        return;
    }

    CloseSocket(socket, SOCKET_CR_CLOSE_EVENT);
}

bool NetworkManager::AppendNewSocket(TealSocket * sock)
{
    if(sock == NULL) return false;

    //新加入的socket，监听他们的读写事件
    bool result = m_socketListener->RegisterSocketEvent(sock, TealSocket::ReadWriteEvent);
    if(result == false)
    {
        LOG_ERROR("Register Socket Event for Addr: %s failed, Error: %s", TealSocket::SocketToStr(sock), strerror(errno));
        return false;
    }

    //加入到序列号map中
    m_socketSeqMap.insert(std::make_pair(sock->GetSequence(), sock));
    return true;
}

void NetworkManager::CloseSocket(TealSocket * socket, int reason)
{
    LOG_INFO("Socket %s Closed with reason: %d", TealSocket::SocketToStr(socket), reason);

    socket->SendCache();
    socket->SetState(SOCKET_STATE_CLOSING);
    m_closeSockets.push_back(socket);
}


void NetworkManager::ProcessSocketSend()
{

}

void NetworkManager::ProcessSocketClose()
{

}

void NetworkManager::CheckSocketTimeout()
{

}

