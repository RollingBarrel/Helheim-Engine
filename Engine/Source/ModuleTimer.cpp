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
	static short frameCounter = 0;
	++frameCounter;

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


	mUpdateTime += mDeltaTime;
	if (mUpdateTime >= 500) {
		if (mFpsLog.size() >= 100) {
			mFpsLog.erase(mFpsLog.begin());
		}
		mFpsLog.push_back(frameCounter * 1000 / (float)mUpdateTime);
		frameCounter = 0;
		mUpdateTime = 0;
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

