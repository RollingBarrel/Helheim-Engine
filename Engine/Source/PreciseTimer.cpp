#include "PreciseTimer.h"
#include "SDL.h"

void PreciseTimer::Start() {
	mInitialCount = SDL_GetPerformanceCounter();
}

long long PreciseTimer::Read() {
	ReadDelta();
	return mTotalTime;
}

long long PreciseTimer::ReadDelta() {
	static Uint64 frequency = SDL_GetPerformanceFrequency();
	Uint64 count = SDL_GetPerformanceCounter();
	Uint64 elapsedTime = (count - mLastCount) / frequency;
	mLastCount = count;
	mTotalTime += elapsedTime;
	return elapsedTime;
}

long long PreciseTimer::Stop() {
	Uint64 finalTime = Read();
	mInitialCount = 0;
	return finalTime;
}