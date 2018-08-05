#include "network_manager.h"
#include "clock_counter.h"
#include "epoll_socket_listener.h"
#include "logger.h"

#define NETWORK_SEQUENCE_START 1

NetworkManager * NetworkManager::Create(int maxClient, ClockCounter * clockCounter)
{
    NetworkManager * nm = new NetworkManager();

    if(clockCounter == NULL)
    {
        clockCounter = &ClockCounter::Instance();
        nm->SetSelfClockCounter(true);
    }
    
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
    m_selfClockCounter = false;
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
    sock->SetConnMark(seq);
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
    m_socketMarkMap.insert(std::make_pair(seq, sock));

    return seq;
}

void NetworkManager::Update()
{
    if(m_selfClockCounter)
    {
        m_clockCounter->Update();
    }

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

    ns->SetConnMark(GetSequenceNum());

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
        return;
    }

    if(readLen == 0)
    {
        CloseSocket(socket, SOCKET_CR_REMOTE_CLOSE);
        return;
    }

    LOG_INFO("Received %d bytes from Addr: %s", readLen, TealSocket::SocketToStr(socket));

    SocketHandler * handler = socket->GetHandler();
    int pkgHeadLen = handler->GetPkgHeadLen();
    int maxPkgLen = handler->GetMaxPkgLen();

    while(true)
    {
        int cachedBytes = socket->RecvDataBytes();
        if(cachedBytes < pkgHeadLen)
        {
            //数据太少，可能还没收全，继续等
            LOG_INFO("Package Too short, continue wait more cached");
            break;
        }

        const char * readDataPtr = socket->GetRecvDataPtr();

        int pkgLen, cmdId;
        handler->GetMsgHeadData(readDataPtr, pkgLen, cmdId);
        if(pkgLen < pkgHeadLen || pkgLen > maxPkgLen)
        {
            //收到错误的消息，客户端不地道，关掉它
            LOG_ERROR("Received Invalid Package From: %s, pkgLen: %d not expected: [%d - %d]", 
                TealSocket::SocketToStr(socket), pkgLen, pkgHeadLen, maxPkgLen);
            CloseSocket(socket, SOCKET_CR_INVALID_PKG);
            return;
        }

        if(cachedBytes < pkgLen)
        {
            //还没收全，继续等
            LOG_INFO("Cached Bytes: %d, Pkg len: %d, continue wait all pkg data cached", cachedBytes, pkgLen);
            break;
        }

        handler->HandleMessage(socket->GetConnMark(), cmdId, readDataPtr + pkgHeadLen, pkgLen - pkgHeadLen);
        socket->RemoveRecvData(pkgLen);
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
    m_socketMarkMap.insert(std::make_pair(sock->GetConnMark(), sock));
    return true;
}

void NetworkManager::CloseSocket(TealSocket * socket, int reason)
{
    LOG_INFO("Socket %s Closed with reason: %d", TealSocket::SocketToStr(socket), reason);

    m_closeSockets.push_back(socket);
}


void NetworkManager::ProcessSocketSend()
{

}

void NetworkManager::ProcessSocketClose()
{
    if(m_closeSockets.size() <= 0) return;

    SocketVector::iterator iter = m_closeSockets.begin();
    for( ; iter != m_closeSockets.end(); ++iter)
    {
        TealSocket * socket = *iter;
    
        socket->Close();
        RemoveFromSocketMap(socket);

        TealSocket::Free(socket);
    }
    m_closeSockets.clear();
}

void NetworkManager::CheckSocketTimeout()
{

}

void NetworkManager::RemoveFromSocketMap(TealSocket * socket)
{
    if(socket == NULL) return;

    int marker = socket->GetConnMark();
    SocketMarkMap::iterator iter = m_socketMarkMap.find(marker);
    if(iter != m_socketMarkMap.end())
    {
        m_socketMarkMap.erase(iter);
    }
}

time_t NetworkManager::GetCurrTimeSec()
{
    if(m_clockCounter != NULL)
    {
        return m_clockCounter->GetCurrTimeSec();
    }
    else
    {
        struct timeval currTime;
        gettimeofday(&currTime, NULL);
        return currTime.tv_sec;
    }
}