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
	mTotalTime += elapsedTime * mSpeed;
	return elapsedTime * mSpeed;
}

long Timer::SetSpeed(float speed) {
	long currentTime = ReadDelta();
	mSpeed = speed;
	return currentTime;
}