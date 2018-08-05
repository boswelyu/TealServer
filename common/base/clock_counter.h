#ifndef _TEAL_CLOCK_COUNTER_H_
#define _TEAL_CLOCK_COUNTER_H_

#include "common.h"

class ClockCounter
{
public:
    //每个服务器只需要一个时钟计数器
    static ClockCounter & Instance()
    {
        static ClockCounter instance;
        return instance;
    }

    void Update();

    time_t GetCurrTimeSec();

private:
    ClockCounter();

private:
    struct timeval m_currTime;
};

#endif