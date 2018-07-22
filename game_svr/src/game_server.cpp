#include "common.h"
#include "game_server.h"
#include "config_reader.h"
#include "logger.h"

GameServer::GameServer()
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
    return true;
}

// 游戏主循环
void GameServer::GameLoop()
{
    while(true)
    {

    }
}