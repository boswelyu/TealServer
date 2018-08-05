#include "clock_counter.h"

ClockCounter::ClockCounter()
{

}

void ClockCounter::Update()
{
    gettimeofday(&m_currTime, NULL);
}

time_t ClockCounter::GetCurrTimeSec()
{
    return m_currTime.tv_sec;
}
