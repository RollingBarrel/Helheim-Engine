#include "EngineTimer.h"
#include "SDL.h"

EngineTimer::EngineTimer(): counter(0), stop(false)
{
	perfFrequency = SDL_GetPerformanceFrequency();
}
EngineTimer::~EngineTimer(){}

void EngineTimer::Start()
{
	stop = false;
	counter = SDL_GetPerformanceCounter();
}

float EngineTimer::Read()
{
	if (stop)
	{
		return stopTime;
	}
	unsigned long long curr = SDL_GetPerformanceCounter();
	unsigned long long diff = curr - counter;
	return (float)diff / (float)perfFrequency;
}

float EngineTimer::Stop()
{
	stop = true;
	stopTime = Read();
	return stopTime;
}