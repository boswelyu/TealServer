#ifndef _TEAL_GAME_SERVER_H_
#define _TEAL_GAME_SERVER_H_

#include <string>

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
    void Start();

private:
    GameServer();

    bool LoadConfig(const char * config);
    bool InitLogger();
    bool InitNetwork();

    void GameLoop();

private:
    GameServerConfig m_config;
};
#endif