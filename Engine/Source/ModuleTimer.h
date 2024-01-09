#pragma once
#include "Globals.h"
#include "Module.h"
#include <vector>


class ModuleTimer : public Module
{
public:

    ModuleTimer();
    ~ModuleTimer();

    bool Init();
    update_status Update();
    bool CleanUp();

    long GetDeltaTime() const { return mDeltaTime; }
    float GetFPS() const { return mFpsLog.back(); }

private:
    long mTime = 0;
    long mDeltaTime = 0;
    long mUpdateTime = 0;
    unsigned int mFpsLimit = 120;
    std::vector<float> mFpsLog;

};
