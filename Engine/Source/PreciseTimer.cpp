#include "PreciseTimer.h"
#include "SDL.h"

void PreciseTimer::Start() {
	mLastReadTime = SDL_GetPerformanceCounter();
	SetTimeScale(1.f);
}

void PreciseTimer::StartWithRunTime() {
	mLastReadTime = SDL_GetPerformanceCounter();
	mTotalTime = mLastReadTime;
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

	//Delay the frame so the FPS match the limit if mDeltaTime if  vsync isn't enabled
	if (!mEnabledVsync && (mFpsLimit > 0 && mDeltaTime / mSpeed < (MICRO_IN_SECONDS / mFpsLimit)))
	{
		mFrameDelay = (MICRO_IN_SECONDS / mFpsLimit) - mDeltaTime / mSpeed;

		SDL_Delay(mFrameDelay/1000);

		ReadDelta();	//ReadDelta is called so the next frameDelay is calculated properly (if this isn't done the previous delay will be counted as part of the next frame execution time)

		mDeltaTime = mSpeed * MICRO_IN_SECONDS / mFpsLimit;
	}
	else
	{
		mFrameDelay = 0;
	}

	//Checks if the frame is the slowest of all (doesn't check the first 50 because the first ones are always very slow)
	if (mTotalFrames > 50) {
		SetSlowestFrame();
	}

	//Logs the frames' time 
	if (mMicroLog.size() >= 100) {
		mMicroLog.erase(mMicroLog.begin());
	}
	mMicroLog.push_back(mDeltaTime / mSpeed);

	mUpdateTime += mDeltaTime / mSpeed;

	//Logs the average FPS every half a second
	if (mUpdateTime > 0.5f * MICRO_IN_SECONDS) {
		if (mFpsLog.size() >= 100) {
			mFpsLog.erase(mFpsLog.begin());
		}
		mFpsLog.push_back(mUpdateFrames * MICRO_IN_SECONDS / mUpdateTime);

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
	double elapsedTime = static_cast<double>(count - mLastReadTime) / frequency;
	Uint64 elapsedMicroTime = elapsedTime * MICRO_IN_SECONDS;
	Uint64 convertedTime = elapsedMicroTime * mSpeed;
	mLastReadTime = count;
	mRealTime += elapsedMicroTime;
	mTotalTime += convertedTime;
	return convertedTime;
}

void PreciseTimer::ResetVariables() {
	mLastReadTime = 0;

	mDeltaTime = 0;
	mFpsLimit = 60;

	mUpdateTime = 0;
	mUpdateFrames = 0;

	mNewSpeed = 1.f;
	mChangeSpeed = false;

	mRealTime = 0;
	mTotalTime = 0;

	mTotalFrames = 0;

	mFpsLog.clear();
	mMicroLog.clear();

	mUpdateFpsLog = false;

	mLowestFps = mFpsLimit;
	mLowestFpsTime = 0;

	mFrameDelay = 0;

	mSlowestFrameTime = 0;
	mSlowestFrame = 0;
}

void PreciseTimer::Pause() {
	SetTimeScale(0.f);
}

void PreciseTimer::Resume() {
	SetTimeScale(1.f);
	mDeltaTime = ReadDelta();
}

long long PreciseTimer::Stop() {
	Uint64 finalTime = Read();

	//We reset all variables
	ResetVariables();

	Pause();

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

void PreciseTimer::SetVsyncStatus(bool vsyncStatus) {
	mEnabledVsync = vsyncStatus;
	SDL_GL_SetSwapInterval(mEnabledVsync ? 1 : 0);
}