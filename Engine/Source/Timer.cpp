#include "Timer.h"
#include "SDL.h"


void Timer::Start() {
	mLastReadTime = SDL_GetTicks();
}

void Timer::StartWithRunTime() {
	mLastReadTime = SDL_GetTicks();
	mTotalTime = mLastReadTime;
}

long Timer::Read() {
	ReadDelta();
	return mTotalTime;
}

long Timer::ReadDelta() {
	Uint32 newTime = SDL_GetTicks();
	Uint32 elapsedTime = newTime - mLastReadTime;
	mLastReadTime = newTime;
	long convertedTime = static_cast<long>(elapsedTime * mSpeed);
	mTotalTime += convertedTime;
	return convertedTime;
}

long Timer::SetSpeed(float speed) {
	long currentTime = ReadDelta();
	mSpeed = speed;
	return currentTime;
}