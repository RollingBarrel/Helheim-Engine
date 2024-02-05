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
    update_status PreUpdate();
    bool CleanUp();

    long GetRealDelta() const { return mDeltaTime; }
    long GetGameDelta() const { return mGameDelta; }

    float GetFPS() const 
    { 
        if (!mFpsLog.empty()) 
        {
            return mFpsLog.back();
        }
        return 0; 
    }
    unsigned int GetFpsLimit() const { return mFpsLimit; }

    std::vector<float> GetFpsLog() const { return mFpsLog; }
    bool UpdateFpsLog() const { return mUpdateFpsLog; }
    void FpsLogUpdated() { mUpdateFpsLog = false; }

    void SetTimeScale(float speed);

private:
    Timer* mGameClock = nullptr;
    Timer* mRealClock = nullptr;
    long mGameDelta = 0;
    unsigned long mDeltaTime = 0;
    long mUpdateTime = 0;
    bool mChangeSpeed = false;
    float mNewSpeed = 1;
    unsigned int mFpsLimit = 60;
    std::vector<float> mFpsLog;
    bool mUpdateFpsLog = false;

};
