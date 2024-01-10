#pragma once
#include "Globals.h"
#include "Module.h"
#include <vector>

class Timer;

class ModuleTimer : public Module
{
public:

    ModuleTimer();
    ~ModuleTimer();

    bool Init();
    update_status Update();
    bool CleanUp();

    long GetRealDeltaTime() const { return mDeltaTime; }
    long GetGameDeltaTime() const { return mDeltaTime * mGameClockSpeed; }
    float GetFPS() const { return mFpsLog.back(); }

    void SetGameSpeed(float speed) { mGameClockSpeed = speed; }

private:
    Timer* mGameClock = nullptr;
    long mDeltaTime = 0;
    long mUpdateTime = 0;
    unsigned int mFpsLimit = 60;
    std::vector<float> mFpsLog;

};
