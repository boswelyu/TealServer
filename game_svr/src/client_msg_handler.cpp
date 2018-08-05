#include "client_msg_handler.h"
#include "logger.h"

ClientMsgHandler::ClientMsgHandler()
{

}

void ClientMsgHandler::HandleMessage(int conn, int cmdId, const char * pkg, int pkgLen)
{
    LOG_INFO("Received Message From Client, PkgLen: %d", pkgLen);
    bool ret = false;
    switch(cmdId)
    {
    case PB::CS_CMD_REQ_LOGIN:
    {
        PARSE_PROTO_MSG(PB::CSLoginReq);
        if(ret) ProcessLoginRequest(conn, msg);
    }
        break;
    default:
        LOG_ERROR("Unknown CMDID: %d", cmdId);
    }
}

void ClientMsgHandler::ProcessLoginRequest(int conn, PB::CSLoginReq & msg)
{
    //TODO: 先不管发什么都返回成功，把conn作为分配的user_id返回去
    PB::SCLoginRes res;
    res.set_login_result(PB::E_LOGIN_SUCCESS);
    res.set_user_id((uint32_t)conn);
}