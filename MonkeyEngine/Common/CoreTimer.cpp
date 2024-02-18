#include "CoreTimer.h"

CoreTimer::CoreTimer()
{
    SecondPerCount = 0.0f;
    DeltaTime = -1.0f;
    BaseTime = 0;
    PausedTime = 0;
    PrevTime = 0;
    CurrTime = 0;
    TimerStopped = false;
    StopTime = 0;

    __int64 countsPerSec;
    QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
    SecondPerCount = 1.0f / (double)countsPerSec;
}

float CoreTimer::TotalTime() const
{
    if (TimerStopped) {
        return (float)(((StopTime - PausedTime) - BaseTime) * SecondPerCount);
    }
    else {
        return (float)(((CurrTime - PausedTime) - BaseTime) * SecondPerCount);
    }
}

float CoreTimer::DeltatIme() const
{
    return (float)DeltaTime;
}

void CoreTimer::Reset()
{
    __int64 currTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
    BaseTime = currTime;
    PrevTime = currTime;
    StopTime = 0;
    TimerStopped = false;
}

void CoreTimer::Start()
{
    __int64 startTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
    if (TimerStopped) {
        PausedTime += (startTime-StopTime);
        StopTime = 0;
        TimerStopped = false;
    }

}

void CoreTimer::Stop()
{
    if (!TimerStopped) {
        __int64 currTime;
        QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
        StopTime = currTime;
        TimerStopped = true;
    }
}

void CoreTimer::Tick()
{
    if (TimerStopped)
    {
        DeltaTime = 0.0;
        return;
    }
    __int64 currTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
    CurrTime = currTime;
    DeltaTime = (CurrTime - PrevTime) * SecondPerCount;
    PrevTime = CurrTime;
    if (DeltaTime < 0.0)
    {
        DeltaTime = 0.0;
    }
}
