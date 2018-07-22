#include "common.h"
#include "os_base.h"
#include "iomn.h"
#include "game_server.h"

#define IOMN_SOCKET_NAME "@iomn_game"

extern void IomnMenuEntry();

int main(int argc, char * argv[])
{
    if(argc != 2)
    {
        printf("Usage: %s config_file\n", argv[0]);
        return -1;
    }

    char configFile[256] = {0};
    strncpy(configFile, argv[1], sizeof(configFile));

    StartDaemon();
    
    GameServer & gameSvr = GameServer::Instance();
    if(gameSvr.Init(configFile) == false) return -1;

    pthread_t iomnTid = IomnStart(IOMN_SOCKET_NAME, IomnMenuEntry);

    gameSvr.GameLoop();

    IomnExit(iomnTid);
}