#ifndef _TEAL_EPOLL_SOCKET_LISTENER_H_
#define _TEAL_EPOLL_SOCKET_LISTENER_H_

#include <sys/epoll.h>
#include "common.h"
#include "network_manager.h"

class EpollSocketListener : public ISocketListener
{
public:
    EpollSocketListener();
    virtual ~EpollSocketListener();

    virtual bool Init(int maxfd, NetworkManager * nm);
    virtual bool RegisterSocketEvent(TealSocket * socket, int event);
    virtual void CheckSocketEvents(int timeout);

private:
    NetworkManager * m_manager;
    int m_maxEvents;
    int m_kdpfd;
    struct epoll_event * m_events;
};
#endif