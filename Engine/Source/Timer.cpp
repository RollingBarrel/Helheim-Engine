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
	++mUpdateFrames;
	++mTotalFrames;

	//Calculates the time it takes to execute the last frame
	if (mChangeSpeed) {
		mDeltaTime = SetSpeed(mNewSpeed);
		mChangeSpeed = false;
	}
	else { 
		mDeltaTime = ReadDelta(); 
	}

	//Delay the frame so the FPS match the limit if mDeltaTime 
	if (mFpsLimit > 0 && mDeltaTime < (1000 / mFpsLimit))
	{
		mFrameDelay = (1000 / mFpsLimit) - mDeltaTime;
		SDL_Delay(mFrameDelay);
		mDeltaTime = 1000 / mFpsLimit;
	}
	else 
	{
		mFrameDelay = 0;
	}

	//Checks if the frame is the slowest of all (doesn't check the first 500 because the first ones are always very slow)
	if (mTotalFrames > 50) {
		SetSlowestFrame();
	}

	//Logs the frames' time 
	if (mMsLog.size() >= 100) {
		mMsLog.erase(mMsLog.begin());
	}
	mMsLog.push_back(mDeltaTime);

	//This may have to be changed
	if(mSpeed != 0)
	{
		mUpdateTime += mDeltaTime / mSpeed;
	}
	else 
	{
		mUpdateTime += mDeltaTime;
	}

	//Logs the average FPS every half a second
	if (mUpdateTime >= 500) {
		if (mFpsLog.size() >= 100) {
			mFpsLog.erase(mFpsLog.begin());
		}
		mFpsLog.push_back(mUpdateFrames * 1000 / mUpdateTime);

		//Checks if the average is the lowest
		SetLowestFps();

		mUpdateFrames = 0;
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
	mRealTime += elapsedTime;
	mTotalTime += convertedTime;
	return convertedTime;
}

long Timer::Stop() {
	Uint64 finalTime = Read();
	mLastReadTime = 0;

	SetSpeed(0.f);

	return finalTime;
}

long Timer::SetSpeed(float speed) {
	long currentTime = ReadDelta();
	mSpeed = speed;
	return currentTime;
}

void Timer::SetTimeScale(float speed) {
	mNewSpeed = speed;
	mChangeSpeed = true;
}

void Timer::SetSlowestFrame() {
	if (mDeltaTime > mSlowestFrameTime) {
		mSlowestFrameTime = mDeltaTime;
		mSlowestFrame = mTotalFrames;
	}
}

void Timer::SetLowestFps() {
	if (mFpsLog.back() < mLowestFps) {
		mLowestFps = mFpsLog.back();
		mLowestFpsTime = mRealTime;
	}
}