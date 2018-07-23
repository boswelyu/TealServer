#ifndef _TEAL_LOGGER_H_
#define _TEAL_LOGGER_H_

#include "common.h"
#include "locker.h"

enum LOG_LEVEL
{
    LOG_LEVEL_OFF = 0,             
    LOG_LEVEL_INFO  = 1,
    LOG_LEVEL_DEBUG = 2,
    LOG_LEVEL_WARN = 3,
    LOG_LEVEL_ERROR = 4,
    LOG_LEVEL_FATAL = 5,
    LOG_LEVEL_MAX = 5,
};

#define MAX_TRACE_DEPTH 8

#define LOG_PREFIX_FUNC __FILE__, __LINE__, __FUNCTION__
#define LOG_PREFIX __FILE__, __LINE__

#define CHECK_LOG_LEVEL(level) \
    if(false ==  Logger::Instance().NeedLog(level)) \
    { \
        break; \
    }

#define LOG_INFO(format, ...) \
	do \
    { \
        CHECK_LOG_LEVEL(LOG_LEVEL_INFO); \
	    char tmpFormat__[10*1024];\
	    snprintf(tmpFormat__, sizeof(tmpFormat__), "(%s:%d) (%s): %s", LOG_PREFIX_FUNC, format);\
	    Logger::Instance().Print(LOG_LEVEL_INFO, tmpFormat__, ##__VA_ARGS__);  \
    } while (0)

#define LOG_DEBUG(format, ...) \
	do \
    {\
        CHECK_LOG_LEVEL(LOG_LEVEL_DEBUG); \
	    char tmpFormat__[10*1024];\
	    snprintf(tmpFormat__, sizeof(tmpFormat__), "(%s:%d) (%s): %s", LOG_PREFIX_FUNC, format);\
	    Logger::Instance().Print(LOG_LEVEL_DEBUG, tmpFormat__, ##__VA_ARGS__); \
    } while (0)

#define LOG_WARN(format, ...) \
	do \
    {\
        CHECK_LOG_LEVEL(LOG_LEVEL_WARN); \
	    char tmpFormat__[10*1024];\
	    snprintf(tmpFormat__, sizeof(tmpFormat__), "(%s:%d) (%s): %s", LOG_PREFIX_FUNC, format);\
	    Logger::Instance().Print(LOG_LEVEL_WARN, tmpFormat__, ##__VA_ARGS__); \
    } while (0)

#define LOG_ERROR(format, ...) \
	do \
    { \
        CHECK_LOG_LEVEL(LOG_LEVEL_ERROR); \
	    char tmpFormat__[10*1024];\
	    snprintf(tmpFormat__, sizeof(tmpFormat__), "(%s:%d) (%s): %s", LOG_PREFIX_FUNC, format);\
	    Logger::Instance().Print(LOG_LEVEL_ERROR, tmpFormat__, ##__VA_ARGS__);  \
    } while (0)

#define LOG_FATAL(format, ...) \
	do \
    { \
        CHECK_LOG_LEVEL(LOG_LEVEL_FATAL); \
	    char tmpFormat__[10*1024];\
	    snprintf(tmpFormat__, sizeof(tmpFormat__), "(%s:%d) (%s): %s", LOG_PREFIX_FUNC, format);\
	    Logger::Instance().Print(LOG_LEVEL_FATAL, tmpFormat__, ##__VA_ARGS__);  \
    } while (0)


class Logger
{
public:
    static Logger & Instance()
    {
        static Logger instance;
        return instance;
    }
    virtual ~Logger();

    bool Init(const char * path, const char * serverName);
    void SetLogLevel(int logLevel);

    bool NeedLog(int logLevel);
    void Print(int logLevel, const char * fmt, ...);

private:
    Logger();
    const int MAX_LOG_FILE_SIZE = 40960000;
    static const char * LogLevelStr[];

    void UpdateTimeStr();

private:
    FILE * m_fp;
    int  m_logLevel;
    SpinLock   m_locker;
    char m_currTimeStr[32];

    time_t m_lastSeccond;
    struct tm m_localTime;
};
#endif