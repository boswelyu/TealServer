#ifndef _TEAL_NETWORK_MANAGER_H_
#define _TEAL_NETWORK_MANAGER_H_

#include "socket_handler.h"
#include "clock_counter.h"
#include "teal_socket.h"

#include <unordered_map>

typedef int hconn_t;

class NetworkManager;

class ISocketListener
{
public:
    virtual ~ISocketListener() {}
    virtual bool Init(int maxfd, NetworkManager * nm) = 0;
    virtual bool RegisterSocketEvent(TealSocket * socket, int event) = 0;
    virtual void CheckSocketEvents(int timeout) = 0;
};

class NetworkManager
{
public:
    static NetworkManager * Create(int maxClient, ClockCounter * clockCounter);
    
    int GetSequenceNum();
    virtual ~NetworkManager();

    hconn_t Listen(const char * ip, const char * port, SocketHandler * handler);

    virtual void Update();

    void HandleCloseEvent(TealSocket * socket);
    void HandleAcceptEvent(TealSocket * socket);
    void HandleReadEvent(TealSocket * socket);
    void HandleWriteEvent(TealSocket * socket);

private:
    NetworkManager();

    void SetClockCounter(ClockCounter * clockCounter)
    {
        m_clockCounter = clockCounter;
    }

    void SetSocketListener(ISocketListener * socketListener)
    {
        m_socketListener = socketListener;
    }

private:
    int m_sequence;
    ClockCounter * m_clockCounter;
    ISocketListener * m_socketListener;

    typedef std::unordered_map<int, TealSocket *> SocketSeqMap; 
    SocketSeqMap m_socketSeqMap;                                 //使用Sequence索引的TealSocket
};

#endif