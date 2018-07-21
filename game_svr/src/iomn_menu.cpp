#include "os_base.h"
#include "iomn.h"
#include "game_server.h"

#ifdef ENABLE_PROFILER
#include <gperftools/profiler.h>
#define PROFILE_DUMP_FILE "/tmp/conn_cpu_profile.out"
#endif

void PrintTopMenuHelp();
void ProfilerMenu();

extern char * g_read_buff;
extern char * g_dump_buff;
extern int g_buff_size;

void IomnMenuEntry()
{
    iomn_push("Welcome to IOMN of Connect Server\n");
    PrintTopMenuHelp();

    while(1)
    {
        char * cmd = iomn_gets(g_read_buff, g_buff_size);
        if(cmd == NULL) break;

        if(*cmd == 'q')
        {
            iomn_print("Good Bye!\n");
            break;
        }

        switch(*cmd)
        {
            case 'p':
                ProfilerMenu();
                break;

            default:
                break;
        }

        PrintTopMenuHelp();
    }
}

void PrintTopMenuHelp()
{
    iomn_push("Available Commands are: \n");
    iomn_push("p) Profiler Menu\n");
    iomn_push("q) QUIT\n");
    iomn_print("Input your select:\n");
}

void PrintProfilerHelp()
{
    iomn_push("\nAvailable Commands are:\n");
    iomn_push("s) Start CPU Profile\n");
    iomn_push("e) End CPU Profile\n");
    iomn_push("b) Begin Memory Leak Check\n");
    iomn_push("d) Finish Memory Leak Check and Dump\n");
    iomn_push("q) Quit and back to upper layer menu\n");
    iomn_print("Input your select:\n");
}

bool cpu_profile_started = false;
bool heap_profile_started = false;

void iomn_start_profiler()
{
#if ENABLE_PROFILER
    if(cpu_profile_started == false)
    {
        ProfilerStart(PROFILE_DUMP_FILE);
        cpu_profile_started = true;
    }
#else
    iomn_print("Please Define the ENABLE_PROFILER in iomn_menu as 1 and recompile the program!\n");
#endif
}

void iomn_stop_profiler()
{
#if ENABLE_PROFILER
    if(cpu_profile_started)
    {
        ProfilerStop();
        cpu_profile_started = false;
        iomn_print("CPU Profile result dumped to file: %s\n", PROFILE_DUMP_FILE);
    }
#else
    iomn_print("Please Define the ENABLE_PROFILER in iomn_menu as 1 and recompile the program!\n");
#endif
}

void ProfilerMenu()
{
    PrintProfilerHelp();

    while(1)
    {
        char * cmd = iomn_gets(g_read_buff, g_buff_size);
        if(cmd == NULL || *cmd == 'q') break;

        switch(*cmd)
        {
            case 's':
                iomn_start_profiler();
                break;
            case 'e':
                iomn_stop_profiler();
                break;
            case 'b':
                iomn_print("TODO: Start Memory Leak Collect\n");
                break;
            case 'd':
                iomn_print("TODO: Finish Memory Leak Collect\n");
                break;
            default:
                iomn_print("Invalid Command: %c\n", *cmd);
                break;
        }

        PrintProfilerHelp();
    }
}
