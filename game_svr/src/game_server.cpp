#include "os_base.h"
#include "game_server.h"
#include "config_reader.h"

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

    return true;
}

bool GameServer::LoadConfig(const char * config)
{
    return true;
}

bool GameServer::InitLogger()
{
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