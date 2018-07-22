#include "epoll_socket_listener.h"
#include "logger.h"

EpollSocketListener::EpollSocketListener()
    : m_events(NULL)
{

}

EpollSocketListener::~EpollSocketListener()
{

}

bool EpollSocketListener::Init(int maxfd)
{
    m_maxEvents = maxfd + 1;
    m_events = new epoll_event[m_maxEvents];
    if(m_events == NULL)
    {
        LOG_ERROR("Create %d epoll_event Failed", maxfd + 1);
        return false;
    }

    m_kdpfd = epoll_create(m_maxEvents);
    if(m_kdpfd < 0)
    {
        LOG_ERROR("epoll_create with max events: %d failed", m_maxEvents);
        delete []m_events;
        return false;
    }
    return true;
}

