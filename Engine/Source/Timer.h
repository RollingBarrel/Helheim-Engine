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

    void SetTimeScale(float speed);

    long GetRealDelta() const { return mDeltaTime; }

    float GetFPS() const
    {
        if (!mFpsLog.empty())
        {
            return mFpsLog.back();
        }
        return 0;
    }

    unsigned long GetTotalTime() { return mTotalTime; }
    unsigned int GetTotalFrames() { return mTotalFrames; }

    unsigned int GetFpsLimit() const { return mFpsLimit; }
    void SetFpsLimit(unsigned int limit) { mFpsLimit = limit; }

    std::vector<float> GetFpsLog() const { return mFpsLog; }
    std::vector<unsigned long> GetMsLog() const { return mMsLog; }

    bool UpdateFpsLog() const { return mUpdateFpsLog; }
    void FpsLogUpdated() { mUpdateFpsLog = false; }

private:
	float mSpeed = 1.0f;
    float mNewSpeed = 1.0f;

	long mLastReadTime = 0;         //Time the last frame was read

    unsigned long mDeltaTime = 0;   //Ms of the last frame

	long mTotalTime = 0;            //Total time since start of Timer
    unsigned int mTotalFrames = 0;  //Total frames since start of Timer

    unsigned int mFpsLimit = 60;    //Limit of FPS

    long mUpdateTime = 0;           //Time since last FPS calculation (reset every 500 ms)
    unsigned int mUpdateFrames = 0; //Frames since last FPS calculation (reset every 500 ms)

    bool mChangeSpeed = false;

    std::vector<float> mFpsLog;         //Log of the last 100 FPS calculated
    std::vector<unsigned long> mMsLog;  //Log of the time of the last 100 frames

    bool mUpdateFpsLog = false;         //True if enough time has passed (500ms) to calculate FPS

};