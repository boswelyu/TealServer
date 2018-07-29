#ifndef _TEAL_SOCKET_HANDLER_H
#define _TEAL_SOCKET_HANDLER_H

#include <arpa/inet.h>

class NetworkManager;
class TealSocket;

class SocketHandler
{
public:
    virtual ~SocketHandler() {}
    virtual bool CanAccept(struct sockaddr * addr, int len) { return true; }
    virtual void OnAccept(NetworkManager *manager, TealSocket * listener, TealSocket * nsock) {}
    virtual void OnConnec(NetworkManager * manager, TealSocket * nsock) {}
    virtual void OnClosed(NetworkManager * manager, TealSocket * nsock) {}
    
protected:
    SocketHandler() {}
};
#endif