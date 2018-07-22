#ifndef _TEAL_NETWORK_MANAGER_H_
#define _TEAL_NETWORK_MANAGER_H_

#include "socket_handler.h"
#include "clock_counter.h"

typedef int hconn_t;

class ISocketListener
{
public:
    virtual ~ISocketListener() {}
    virtual bool Init(int maxfd) = 0;
};

class NetworkManager
{
public:
    static NetworkManager * Create(int maxClient, ClockCounter * clockCounter);

    virtual ~NetworkManager();

    hconn_t Listen(const char * ip, const char * port, SocketHandler * handler);

    void Close(hconn_t conn);

    virtual void Update();

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
    ClockCounter * m_clockCounter;
    ISocketListener * m_socketListener;
};

#endif