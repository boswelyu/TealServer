#ifndef _TEAL_CLIENT_MSG_HANDLER_H_
#define _TEAL_CLIENT_MSG_HANDLER_H_

#include "socket_handler.h"

class ClientMsgHandler : public SocketHandler
{
public:
    static ClientMsgHandler & Instance()
    {
        static ClientMsgHandler instance;
        return instance;
    }

    virtual ~ClientMsgHandler() {}

private:
    ClientMsgHandler();
};
#endif