#pragma once
#include "ModuleTimer.h"
#include "SDL.h"
#include "Application.h"

ModuleTimer::ModuleTimer() {}
ModuleTimer::~ModuleTimer() {}

bool ModuleTimer::Init() {
	mFpsLog.reserve(100);
	mTime = SDL_GetTicks();
	mUpdateTime = mTime;
	return true;
}
update_status ModuleTimer::Update() {
	static short frameCounter = 0;
	++frameCounter;

	long newTime = SDL_GetTicks();
	mDeltaTime = newTime - mTime;

	if (mFpsLimit > 0 && mDeltaTime < (1000 / mFpsLimit))
	{
		SDL_Delay((1000 / mFpsLimit) - mDeltaTime);
		mDeltaTime = (SDL_GetTicks() - mTime);
	}

	mTime = SDL_GetTicks();

	long timeSinceUpdate = newTime - mUpdateTime;
	if (timeSinceUpdate >= 500) {
		if (mFpsLog.size() >= 100) {
			mFpsLog.erase(mFpsLog.begin());
		}
		mFpsLog.push_back(frameCounter * 1000 / (float)timeSinceUpdate);
		frameCounter = 0;
		mUpdateTime = SDL_GetTicks();
	}


	return UPDATE_CONTINUE;
}

bool ModuleTimer::CleanUp() {
	return true;
}