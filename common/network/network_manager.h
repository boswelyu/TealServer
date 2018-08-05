#ifndef _TEAL_NETWORK_MANAGER_H_
#define _TEAL_NETWORK_MANAGER_H_

#include "socket_handler.h"
#include "clock_counter.h"
#include "teal_socket.h"

#include <unordered_map>
#include <vector>

typedef int hconn_t;

class NetworkManager;

class ISocketListener
{
public:
    virtual ~ISocketListener() {}
    virtual bool Init(int maxfd, NetworkManager * nm) = 0;
    virtual bool RegisterSocketEvent(TealSocket * socket, int event) = 0;
    virtual bool UpdateSocketEvent(TealSocket * socket, int eflags) = 0;
    virtual void CheckSocketEvents(int timeout) = 0;
};

class NetworkManager
{
public:
    static NetworkManager * Create(int maxClient, ClockCounter * clockCounter);
    
    int GetSequenceNum();
    virtual ~NetworkManager();

    hconn_t Listen(const char * ip, const char * port, SocketHandler * handler, int sendCacheSize, int recvCacheSize, int timeout);

    virtual void Update();

    void SetClockCounter(ClockCounter * clockCounter) { m_clockCounter = clockCounter; }
    void SetSelfClockCounter(bool flag) { m_selfClockCounter = flag; }

    void HandleCloseEvent(TealSocket * socket);
    void HandleAcceptEvent(TealSocket * socket);
    void HandleReadEvent(TealSocket * socket);
    void HandleWriteEvent(TealSocket * socket);

    time_t GetCurrTimeSec();

private:
    NetworkManager();

    void SetSocketListener(ISocketListener * socketListener)
    {
        m_socketListener = socketListener;
    }

    bool AppendNewSocket(TealSocket * socket);
    void CloseSocket(TealSocket * socket, int reason);
    void RemoveFromSocketMap(TealSocket * socket);

    void ProcessSocketSend();
    void ProcessSocketClose();
    void CheckSocketTimeout();

private:
    int  m_sequence;
    bool m_selfClockCounter;

    ClockCounter * m_clockCounter;
    ISocketListener * m_socketListener;

    typedef std::unordered_map<int, TealSocket *> SocketMarkMap; 
    SocketMarkMap m_socketMarkMap;                                 //使用内部标记索引的TealSocket

    typedef std::vector<TealSocket *> SocketVector;
    SocketVector m_closeSockets;
    
};

#endif