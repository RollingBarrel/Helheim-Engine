#pragma once
#include "ModuleTimer.h"
#include "Timer.h"
#include "SDL.h"

ModuleTimer::ModuleTimer() {}
ModuleTimer::~ModuleTimer() {}

bool ModuleTimer::Init() {
	mGameClock = new Timer();
	mRealClock = new Timer();
	mGameClock->Start();
	mRealClock->StartWithRunTime();
	return true;
}

update_status ModuleTimer::PreUpdate() {
	//Logs the average FPS in mFpsLog every 500 ms
	static short frameCounter = 0;
	++frameCounter;
	++mTotalFrames;

	mDeltaTime = mRealClock->ReadDelta();
	if (mChangeSpeed) {
		mGameDelta = mGameClock->SetSpeed(mNewSpeed);
		mChangeSpeed = false;
	}
	else { mGameDelta = mGameClock->ReadDelta(); }

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


	return UPDATE_CONTINUE;
}

bool ModuleTimer::CleanUp() {
	delete mRealClock;
	delete mGameClock;
	return true;
}

void ModuleTimer::SetTimeScale(float speed) {
	mNewSpeed = speed;
	mChangeSpeed = true;
}

