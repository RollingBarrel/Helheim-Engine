#include "Timer.h"
#include "SDL.h"

void Timer::Start() {
	mLastReadTime = SDL_GetTicks();
}

long Timer::Read() {
	ReadDelta();
	return mTotalTime;
}

long Timer::ReadDelta() {
	Uint32 newTime = SDL_GetTicks();
	Uint32 elapsedTime = newTime - mLastReadTime;
	mLastReadTime = newTime;
	mTotalTime += elapsedTime * speed;
	return elapsedTime;
}

long Timer::SetSpeed(float speed) {
	long currentTime = Read();
	speed = speed;
	return currentTime;
}

void Timer::StartPrecise() {
	mInitialCount = SDL_GetPerformanceCounter();
}

long long Timer::ReadPrecise() {
	if (mInitialCount != 0) {
		static Uint64 frequency = SDL_GetPerformanceFrequency();
		Uint64 elapsedCount = SDL_GetPerformanceCounter() - mInitialCount;
		mElapsedNanoseconds = elapsedCount / frequency;
	}
	return mElapsedNanoseconds;
}

long long Timer::StopPrecise() {
	uint64_t finalTime = ReadPrecise();
	mInitialCount = 0;
	return finalTime;
}