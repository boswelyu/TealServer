#include <execinfo.h>
#include "logger.h"

const char * Logger::LogLevelStr[] =
{
    "OFF",
    "INFO",
    "DEBUG",
    "WARN",
    "ERROR",
    "FATAL",
};

Logger::Logger() : m_fp(NULL)
{
    memset(m_currTimeStr, 0, sizeof(m_currTimeStr));
}

Logger::~Logger()
{
    if(m_fp != NULL)
    {
        fflush(m_fp);
        fclose(m_fp);
        m_fp = NULL;
    }
}

bool Logger::Init(const char * path, const char * serverName)
{
    UpdateTimeStr();

    char timeStr[128];
    strncpy(timeStr, m_currTimeStr, 10);
    timeStr[10] = 0;                //只取到天

    int pathLen = strlen(path);

    char fullPath[1024];
    int index = 0;

    while(true)
    {
        if(path[pathLen - 1] == '/')
        {
            snprintf(fullPath, sizeof(fullPath), "%s%s.%s.%d.log", path, serverName, timeStr, index);
        }
        else
        {
            snprintf(fullPath, sizeof(fullPath), "%s/%s.%s.%d.log", path, serverName, timeStr, index);
        }

        struct stat fs;
        if( stat(fullPath, &fs) >= 0 && fs.st_size >= MAX_LOG_FILE_SIZE)
        {
            index++;
            continue;   
        }

        //文件不存在或者还没满，继续使用这个文件
        m_fp = fopen(fullPath, "a+b");
        if(m_fp == NULL)
        {
            printf("Open Log File (%s) Failed, Error: %s\n", fullPath, strerror(errno));
            return false;
        }
        break;
    }
    return true;
}

void Logger::SetLogLevel(int level)
{
    if(level >= 0 && level <= LOG_LEVEL_MAX)
    {
        m_logLevel = level;
    }
    else
    {
        printf("ERROR: Invalid Log level: %d, Log Level Set to default 2\n", level);
        m_logLevel = 2;
    }
}

bool Logger::NeedLog(int logLevel)
{
    if(logLevel <= m_logLevel)
    {
        return true;
    }
    return false;
}

void Logger::UpdateTimeStr()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    struct tm localTime;
    localtime_r(&tv.tv_sec, &localTime);

    snprintf(m_currTimeStr, sizeof(m_currTimeStr), "%04d-%02d-%02d %02d:%02d:%02d.%06ld",
        localTime.tm_year + 1900, localTime.tm_mon + 1, localTime.tm_mday, 
        localTime.tm_hour, localTime.tm_min, localTime.tm_sec, tv.tv_usec);
}    

void Logger::Print(int logLevel, const char * fmt, ...)
{
    Locker locker(&m_locker);

    UpdateTimeStr();

    char buffer[4096];
    snprintf(buffer, sizeof(buffer), "[%s][%s]%s\n", m_currTimeStr, LogLevelStr[logLevel], fmt);

    va_list ap;
    va_start(ap, fmt);
    vfprintf(m_fp, buffer, ap);
    va_end(ap);

    //Error以上的同时记录Backtrace
    if(logLevel >= LOG_LEVEL_ERROR)
    {
        //跟踪调用栈并打印最多8级函数调用，第一级调用一定是这个print函数，所以忽略
        void * ip_values[MAX_TRACE_DEPTH + 1];
        int size = backtrace(ip_values, MAX_TRACE_DEPTH + 1);

        int len = 0;
        for(int i = 1; i <= MAX_TRACE_DEPTH && i < size; i++)
        {
            len += snprintf(buffer + len, sizeof(buffer) - len, "%p ", ip_values[i]);
        }
        fprintf(m_fp, "\t\tBacktrace: %s\n", buffer);
    }

    fflush(m_fp);
}