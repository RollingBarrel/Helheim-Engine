#include "Timer.h"
#include "SDL.h"


void Timer::Start() {
	mLastReadTime = SDL_GetTicks();
}

void Timer::StartWithRunTime() {
	mLastReadTime = SDL_GetTicks();
	mTotalTime = mLastReadTime;
}

void Timer::Update()
{
	static short frameCounter = 0;
	++frameCounter;
	++mTotalFrames;

	if (mChangeSpeed) {
		mDeltaTime = SetSpeed(mNewSpeed);
		mChangeSpeed = false;
	}
	else { mDeltaTime = ReadDelta(); }

	if (mFpsLimit > 0 && mDeltaTime < (1000 / mFpsLimit))
	{
		SDL_Delay((1000 / mFpsLimit) - mDeltaTime);
		mDeltaTime = 1000 / mFpsLimit;
	}

	if (mMsLog.size() >= 100) {
		mMsLog.erase(mMsLog.begin());
	}
	mMsLog.push_back(mDeltaTime);

	mUpdateTime += mDeltaTime;
	if (mUpdateTime >= 500) {
		if (mFpsLog.size() >= 100) {
			mFpsLog.erase(mFpsLog.begin());
		}
		mFpsLog.push_back(frameCounter * 1000 / (float)mUpdateTime);
		frameCounter = 0;
		mUpdateTime = 0;

		mUpdateFpsLog = true;
	}
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