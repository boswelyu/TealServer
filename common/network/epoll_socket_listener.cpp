#include "epoll_socket_listener.h"
#include "logger.h"

EpollSocketListener::EpollSocketListener()
    : m_events(NULL)
{
    m_manager = NULL;
}

EpollSocketListener::~EpollSocketListener()
{

}

bool EpollSocketListener::Init(int maxfd, NetworkManager * nm)
{
    if(nm == NULL)
    {
        LOG_ERROR("No NetworkManager Assign to EpollSocketListener, Init Failed");
        return false;
    }

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
    m_manager = nm;
    return true;
}

bool EpollSocketListener::RegisterSocketEvent(TealSocket * socket, int eflag)
{
    if(socket == NULL) return false;

    epoll_event event;
    memset(&event, 0, sizeof(event));
    
    event.events = EPOLLRDHUP;
    if(eflag & SOCKET_EVENT_ON_READ)
    {
        event.events |= EPOLLIN;
    }
    if(eflag & SOCKET_EVENT_ON_WRITE)
    {
        event.events |= EPOLLOUT;
    }
    event.data.ptr = (void *)socket;

    int ret = epoll_ctl(m_kdpfd, EPOLL_CTL_ADD, socket->GetSocketFd(), &event);
    if(ret < 0 && errno == EEXIST)
    {
        //已经存在，尝试使用修改操作
        ret = epoll_ctl(m_kdpfd, EPOLL_CTL_MOD, socket->GetSocketFd(), &event);
        if(ret < 0)
        {
            return false;
        }
    }
    return true;
}

void EpollSocketListener::CheckSocketEvents(int timeout)
{
    int nfds = epoll_wait(m_kdpfd, m_events, m_maxEvents, timeout);
    if(nfds <= 0) return;

    epoll_event * event = NULL;
    for(int i = 0; i < nfds; i++)
    {
        event = &m_events[i];

        TealSocket * tealSocket = (TealSocket *)event->data.ptr;
        if(tealSocket == NULL)
        {
            LOG_WARN("Got on NULL TealSocket in event data");
            continue;
        }

        int eventFlag = event->events;
        if( (eventFlag & EPOLLERR) || (eventFlag & EPOLLHUP) )
        {
            m_manager->HandleCloseEvent(tealSocket);
            continue;
        }

        if(eventFlag & EPOLLIN)
        {
            if(tealSocket->GetState() == SOCKET_STATE_LISTENING)
            {
                m_manager->HandleAcceptEvent(tealSocket);
            }
            else
            {
                m_manager->HandleReadEvent(tealSocket);
            }
        }

        if(eventFlag & EPOLLOUT)
        {
            m_manager->HandleWriteEvent(tealSocket);
        }
    }
}
