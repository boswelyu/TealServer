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
    virtual void OnConnect(NetworkManager * manager, TealSocket * nsock) {}
    virtual void OnClosed(NetworkManager * manager, TealSocket * nsock) {}

    virtual int GetPkgHeadLen() { return sizeof(uint32_t) + sizeof(uint32_t); }
    virtual int GetMaxPkgLen() { return 32000; }

    void GetMsgHeadData(const char * ptr, int &pkgLen, int &cmdId) 
    {
        uint32_t * data = (uint32_t *)ptr;
        pkgLen = (int)ntohl(*data);

        data++;
        cmdId = (int)ntohl(*data);
    }

    virtual void HandleMessage(int conn, int cmdId, const char * msg, int pkglen) = 0;
    
protected:
    SocketHandler() {}
};
#endif