#include "PreciseTimer.h"
#include "SDL.h"

void PreciseTimer::Start() {
	mLastReadTime = SDL_GetPerformanceCounter();
}

void PreciseTimer::Update()
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
	if (mSpeed != 0)
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

long long PreciseTimer::Read() {
	ReadDelta();
	return mTotalTime;
}

long long PreciseTimer::ReadDelta() {
	static Uint64 frequency = SDL_GetPerformanceFrequency();
	Uint64 count = SDL_GetPerformanceCounter();
	Uint64 elapsedTime = (count - mLastReadTime) / frequency;
	mLastReadTime = count;
	mTotalTime += elapsedTime;
	return elapsedTime;
}

long long PreciseTimer::Stop() {
	Uint64 finalTime = Read();
	mLastReadTime = 0;

	SetSpeed(0.f);

	return finalTime;
}

long long PreciseTimer::SetSpeed(float speed) {
	long long currentTime = ReadDelta();
	mSpeed = speed;
	return currentTime;
}

void PreciseTimer::SetTimeScale(float speed) {
	mNewSpeed = speed;
	mChangeSpeed = true;
}

void PreciseTimer::SetSlowestFrame() {
	if (mDeltaTime > mSlowestFrameTime) {
		mSlowestFrameTime = mDeltaTime;
		mSlowestFrame = mTotalFrames;
	}
}

void PreciseTimer::SetLowestFps() {
	if (mFpsLog.back() < mLowestFps) {
		mLowestFps = mFpsLog.back();
		mLowestFpsTime = mRealTime;
	}
}