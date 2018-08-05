#ifndef _TEAL_CLIENT_MSG_HANDLER_H_
#define _TEAL_CLIENT_MSG_HANDLER_H_

#include "socket_handler.h"
#include "proto_macro.h"
#include "login.pb.h"

class ClientMsgHandler : public SocketHandler
{
public:
    static ClientMsgHandler & Instance()
    {
        static ClientMsgHandler instance;
        return instance;
    }

    virtual ~ClientMsgHandler() {}

    virtual void HandleMessage(int conn, int cmdId, const char * pkg, int pkglen);

private:
    ClientMsgHandler();

    void ProcessLoginRequest(int conn, PB::CSLoginReq & msg);
};
#endif