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

	/*static Uint64 LastTime = SDL_GetPerformanceCounter();
	static Uint64 frequency = SDL_GetPerformanceFrequency();
	static double minElaps = frequency / fpsLimit;
	Uint64 ncounter = SDL_GetPerformanceCounter();
	Uint64 ElapsedTime = (ncounter - LastTime);*/

	mDeltaTime = (SDL_GetTicks() - mTime);
	/*if (ElapsedTime < minElaps)
	{
		SDL_Delay((minElaps - ElapsedTime)*CLOCKS_PER_SEC/frequency);
		ncounter = SDL_GetPerformanceCounter();
		ElapsedTime = (ncounter - LastTime);
	}*/

	if (mDeltaTime < 1000 / mFpsLimit)
	{
		SDL_Delay(1000 / mFpsLimit - mDeltaTime);
		mDeltaTime = (SDL_GetTicks() - mTime);
	}

	mTime = SDL_GetTicks();
	//LastTime = ncounter;

	//double MSPerFrame = (((1000.0f * (double)ElapsedTime) / (double)frequency));
	//double FPS = (double)frequency / (double)ElapsedTime;
	long timeSinceUpdate = mTime - mUpdateTime;
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