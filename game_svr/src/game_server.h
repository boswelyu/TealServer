#ifndef _TEAL_GAME_SERVER_H_
#define _TEAL_GAME_SERVER_H_

#include <string>
#include "clock_counter.h"
#include "network_manager.h"

typedef struct gsConfig
{
    std::string LogFilePath;    //配置文件路径
    int LogLevel;
    int MaxAcceptClient;        //接受的最大连接数

    std::string ListenIP;       //ip
    std::string ListenPort;     //端口
    
}GameServerConfig;

class GameServer
{
public:
    static GameServer & Instance()
    {
        static GameServer instance;
        return instance;
    }
    virtual ~GameServer() {}

    bool Init(const char * config);
    void GameLoop();

private:
    GameServer();

    bool LoadConfig(const char * config);
    bool InitLogger();
    bool InitNetwork();

private:
    GameServerConfig m_config;

    ClockCounter & m_clockCounter;
    NetworkManager * m_pNetworkManager;
};
#endif