#pragma once
#include <vector>

class Timer {
public:
	Timer() = default;

	void Start();
	void Update(); //Log frames and hault to match max FPS

	void StartWithRunTime();

	long Read();
	long ReadDelta();

	long Stop() { SetSpeed(0); }
	long SetSpeed(float speed);

    long GetRealDelta() const { return mDeltaTime; }

    float GetFPS() const
    {
        if (!mFpsLog.empty())
        {
            return mFpsLog.back();
        }
        return 0;
    }

    unsigned int GetFpsLimit() const { return mFpsLimit; }
    void SetFpsLimit(unsigned int limit) { mFpsLimit = limit; }

    std::vector<float> GetFpsLog() const { return mFpsLog; }
    std::vector<unsigned long> GetMsLog() const { return mMsLog; }

    bool UpdateFpsLog() const { return mUpdateFpsLog; }
    void FpsLogUpdated() { mUpdateFpsLog = false; }

    unsigned long GetTotalTime() { return mTotalTime; }
    unsigned int GetTotalFrames() { return mTotalFrames; }

private:
	float mSpeed = 1.0f;
    float mNewSpeed = 1.0f;
	long mLastReadTime = 0;
    unsigned int mTotalFrames = 0;
	long mTotalTime = 0;

    unsigned long mDeltaTime = 0;
    long mUpdateTime = 0;

    bool mChangeSpeed = false;

    std::vector<float> mFpsLog;
    std::vector<unsigned long> mMsLog;
    bool mUpdateFpsLog = false;

    unsigned int mFpsLimit = 60;

};