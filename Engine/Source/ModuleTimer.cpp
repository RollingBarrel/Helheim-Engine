#pragma once
#include "ModuleTimer.h"
#include "SDL.h"
#include "Application.h"
#include <ctime>

ModuleTimer::ModuleTimer() {}
ModuleTimer::~ModuleTimer() {}

bool ModuleTimer::Init() {
	mFpsLog.reserve(101);
	mTime = clock();
	mUpdateTime = mTime;
	return true;
}
update_status ModuleTimer::Update() {
	static short frameCounter = 0;
	++frameCounter;

	/*static Uint64 LastTime = SDL_GetPerformanceCounter();
	static Uint64 frequency = SDL_GetPerformanceFrequency();
	static double minElaps = frequency / fpsLimit;
	Uint64 ncounter = SDL_GetPerformanceCounter();
	Uint64 ElapsedTime = (ncounter - LastTime);*/

	mDeltaTime = (clock() - mTime);
	/*if (ElapsedTime < minElaps)
	{
		SDL_Delay((minElaps - ElapsedTime)*CLOCKS_PER_SEC/frequency);
		ncounter = SDL_GetPerformanceCounter();
		ElapsedTime = (ncounter - LastTime);
	}*/

	if (mDeltaTime < 1000 / mFpsLimit)
	{
		SDL_Delay(1000 / mFpsLimit - mDeltaTime);
		mDeltaTime = (clock() - mTime);
	}

	mTime = clock();
	//LastTime = ncounter;

	//double MSPerFrame = (((1000.0f * (double)ElapsedTime) / (double)frequency));
	//double FPS = (double)frequency / (double)ElapsedTime;
	long timeSinceUpdate = mTime - mUpdateTime;
	if (timeSinceUpdate >= 500) {
		mFpsLog.push_back(frameCounter * CLOCKS_PER_SEC / timeSinceUpdate);
		if (mFpsLog.size() > 100) {
			mFpsLog.erase(mFpsLog.begin());
		}
		frameCounter = 0;
		mUpdateTime = clock();
	}


	return UPDATE_CONTINUE;
}

bool ModuleTimer::CleanUp() {
	return true;
}