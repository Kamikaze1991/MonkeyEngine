#ifndef _CORE_TIMER_H_
#define _CORE_TIMER_H_
#include "CoreUtil.h"

class CoreTimer {
private:
	double SecondPerCount;
	double DeltaTime;

	__int64 BaseTime;
	__int64 PausedTime;
	__int64 StopTime;
	__int64 PrevTime;
	__int64 CurrTime;

	bool TimerStopped;

public:
	CoreTimer();

	float TotalTime()const;
	float DeltatIme()const;

	void Reset();
	void Start();
	void Stop();
	void Tick();
};
#endif

