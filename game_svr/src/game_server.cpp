#include "os_base.h"
#include "game_server.h"
#include "config_reader.h"

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

void GameServer::Start()
{
    while(true)
    {
        GameLoop();
    }
}