#include "Timer.h"
#include "Application.h"
#include "SDL.h"


void Timer::Start() 
{
	mLastReadTime = SDL_GetTicks();
	SetTimeScale(1.f);
}

void Timer::StartWithRunTime() 
{
	mLastReadTime = SDL_GetTicks();
	mTotalTime = mLastReadTime;
}

void Timer::Update()
{
	++mUpdateFrames;
	++mTotalFrames;

	if (mChangeSpeed) 
	{
		mDeltaTime = SetSpeed(mNewSpeed);
		mChangeSpeed = false;
	}
	else 
	{ 
		mDeltaTime = ReadDelta(); 
	}

	//Delay the frame so the FPS match the limit if mDeltaTime if vsync isn't enabled
	if (!mEnabledVsync && mFpsLimitEnabled && (mDeltaTime / mSpeed < (MILLI_IN_SECONDS / mFpsLimit)))
	{
		mFrameDelay = (MILLI_IN_SECONDS / mFpsLimit) - mDeltaTime / mSpeed;

		SDL_Delay(mFrameDelay);

		ReadDelta();	//ReadDelta is called so the next frameDelay is calculated properly (if this isn't done the previous delay will be counted as part of the next frame execution time)

		mDeltaTime = mSpeed * MILLI_IN_SECONDS / mFpsLimit;
	}
	else 
	{
		mFrameDelay = 0;
	}

	//Checks if the frame is the slowest of all (doesn't check the first 50 because the first ones are always very slow)
	if (mTotalFrames > 50) 
	{
		SetSlowestFrame();
	}

	//Logs the frames' time 
	if (mMsLog.size() >= 100) 
	{
		mMsLog.erase(mMsLog.begin());
	}
	mMsLog.push_back(mDeltaTime / mSpeed);

	mUpdateTime += mDeltaTime / mSpeed;

	//Logs the average FPS every half a second
	if (mUpdateTime > 0.5f * MILLI_IN_SECONDS) 
	{
		if (mFpsLog.size() >= 100) 
		{
			mFpsLog.erase(mFpsLog.begin());
		}
		mFpsLog.push_back(mUpdateFrames * MILLI_IN_SECONDS / mUpdateTime);

		//Checks if the average is the lowest
		SetLowestFps();

		mUpdateFrames = 0;
		mUpdateTime = 0;

		mUpdateFpsLog = true;
	}

	if(App->GetCurrentClock() == this ) SDL_GL_SetSwapInterval(mEnabledVsync ? 1 : 0);
}

long Timer::Read() 
{
	ReadDelta();
	return mTotalTime;
}

long Timer::ReadDelta() 
{
	Uint32 newTime = SDL_GetTicks();
	Uint32 elapsedTime = newTime - mLastReadTime;
	mLastReadTime = newTime;
	long convertedTime = static_cast<long>(elapsedTime * mSpeed);
	mRealTime += elapsedTime;
	mTotalTime += convertedTime;
	return convertedTime;
}

void Timer::ResetVariables() 
{
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
	mMsLog.clear();

	mUpdateFpsLog = false;

	mLowestFps = mFpsLimit;
	mLowestFpsTime = 0;

	mFrameDelay = 0;

	mSlowestFrameTime = 0;
	mSlowestFrame = 0;
}

void Timer::Pause() 
{ 
	SetTimeScale(0.f); 
}

void Timer::Resume() 
{
	SetTimeScale(1.f);
	mDeltaTime = ReadDelta();
}

long Timer::Stop() 
{
	Uint32 finalTime = Read();

	//We reset all variables
	ResetVariables();

	Pause();

	return finalTime;
}

long Timer::SetSpeed(float speed) 
{
	long currentTime = ReadDelta();
	mSpeed = speed;
	return currentTime;
}

void Timer::SetTimeScale(float speed) 
{
	mNewSpeed = speed;
	mChangeSpeed = true;
}

void Timer::SetSlowestFrame() 
{
	if (mDeltaTime > mSlowestFrameTime) 
	{
		mSlowestFrameTime = mDeltaTime;
		mSlowestFrame = mTotalFrames;
	}
}

void Timer::SetLowestFps() 
{
	if (mFpsLog.back() < mLowestFps) 
	{
		mLowestFps = mFpsLog.back();
		mLowestFpsTime = mRealTime;
	}
}

void Timer::SetVsyncStatus(bool vsyncStatus) 
{
	mEnabledVsync = vsyncStatus;
}