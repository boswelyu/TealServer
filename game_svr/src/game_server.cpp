#include "common.h"
#include "game_server.h"
#include "config_reader.h"
#include "logger.h"
#include "client_msg_handler.h"

GameServer::GameServer() : m_clockCounter(ClockCounter::Instance())
{
}

bool GameServer::Init(const char * config)
{
    if( !LoadConfig(config) )
    {
        printf("GameServer Load Config (%s) Failed\n", config);
        return false;
    }

    if( !InitLogger() )
    {  
        printf("Game Server InitLogger Failed\n");
        return false;
    }

    if( !InitNetwork() )
    {
        printf("Game Server InitNetwork Failed\n");
        return false;
    }

    LOG_INFO("GameServer Inited Success!");

    return true;
}

bool GameServer::LoadConfig(const char * config)
{
    printf("Loading Config from file: %s\n", config);

    ConfigReader reader;
    if(reader.LoadConfig(config) == false)
    {
        printf("Load Config from %s Failed\n", config);
        return false;
    }

    m_config.LogFilePath = reader.GetStringValue("LogFilePath");
    m_config.LogLevel = reader.GetIntValue("LogLevel");
    m_config.MaxAcceptClient = reader.GetIntValue("MaxAcceptClient");        //接受的最大连接数

    m_config.ListenIP = reader.GetStringValue("ListenIP");       //ip
    m_config.ListenPort = reader.GetStringValue("ListenPort");     //端口

    m_config.SendCacheSize = reader.GetIntValue("SendCacheSize", 512000);
    m_config.RecvCacheSize = reader.GetIntValue("RecvCacheSize", 256000);
    m_config.Timeout = reader.GetIntValue("Timeout", 60);

    return true;
}

bool GameServer::InitLogger()
{
    if(Logger::Instance().Init(m_config.LogFilePath.c_str(), "game_svr") == false)
    {
        printf("Logger Init Failed, Error: %s\n", strerror(errno));
        return false;
    }

    Logger::Instance().SetLogLevel(m_config.LogLevel);
    return true;
}

bool GameServer::InitNetwork()
{
    m_pNetworkManager = NetworkManager::Create(m_config.MaxAcceptClient, &m_clockCounter);
    if(m_pNetworkManager == NULL)
    {
        LOG_ERROR("GameServer Failed to Create NetworkManager when InitNetwork");
        return false;
    }

    ClientMsgHandler &clientMsgHandler = ClientMsgHandler::Instance();
    int conn = m_pNetworkManager->Listen(m_config.ListenIP.c_str(), m_config.ListenPort.c_str(), &clientMsgHandler,
        m_config.SendCacheSize, m_config.RecvCacheSize, m_config.Timeout);
    if(conn < 0)
    {
        LOG_ERROR("GameServer Failed to Listen on: %s:%s, Error: %s", m_config.ListenIP.c_str(), m_config.ListenPort.c_str(), strerror(errno));
        return false;
    }

    //TODO: Set Connection parameters

    printf("GameServer Listening on: %s:%s\n", m_config.ListenIP.c_str(), m_config.ListenPort.c_str());
    return true;
}

// 游戏主循环
void GameServer::GameLoop()
{
    while(true)
    {
        m_clockCounter.Update();
        m_pNetworkManager->Update();
    }
}