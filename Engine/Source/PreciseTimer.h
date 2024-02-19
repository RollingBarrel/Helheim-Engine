#pragma once
#include <vector>

#define MICRO_IN_SECONDS 1000000

class PreciseTimer {
public:
    PreciseTimer() = default;

    void Start();
    void StartWithRunTime();

    void Update(); //Logs frames and statistics and haults the application to match max FPS

    long long Read();
    long long ReadDelta();

    long long Stop();
    void Pause();
    void Resume();

    float GetSpeed() const { return mSpeed; }
    long long SetSpeed(float speed);
    float GetNewSpeed() const { return mNewSpeed; }
    void SetTimeScale(float speed);                     //Similar to SetSpeed but without recalculating current time (that has to be done in Timer::Update())

    long long GetDelta() const { return mDeltaTime; }

    float GetFPS() const
    {
        if (!mFpsLog.empty())
        {
            return mFpsLog.back();
        }
        return 0;
    }

    unsigned long long GetRealTime() const { return mRealTime; }
    unsigned long long GetTotalTime() const { return mTotalTime; }
    unsigned int GetTotalFrames() const { return mTotalFrames; }
    void SetTotalFrames(unsigned int gameFrames) { mTotalFrames += gameFrames; }    //Adds the frames of the game execution to the total frames of the engine

    unsigned int GetFpsLimit() const { return mFpsLimit; }
    void SetFpsLimit(unsigned int limit) { mFpsLimit = limit; }

    std::vector<float> GetFpsLog() const { return mFpsLog; }
    std::vector<unsigned long long> GetMsLog() const { return mMicroLog; }

    bool UpdateFpsLog() const { return mUpdateFpsLog; }
    void FpsLogUpdated() { mUpdateFpsLog = false; }

    float GetLowestFPS() const { return mLowestFps; }
    long long GetLowestFpsTime() const { return mLowestFpsTime; }
    void SetLowestFps();

    long long GetFrameDelay() const { return mFrameDelay; }

    long long GetSlowestFrameTime() const { return mSlowestFrameTime; }
    int GetSlowestFrame() const { return mSlowestFrame; }
    void SetSlowestFrame();

    bool GetVsyncStatus() const { return mEnabledVsync; }
    void SetVsyncStatus(bool vsyncStatus);

    float GetTimerPrecision() const { return (float)MICRO_IN_SECONDS; }

private:

    void ResetVariables();

    float mSpeed = 1.0f;
    float mNewSpeed = 1.0f;

    bool mChangeSpeed = false;

    long long mLastReadTime = 0;                //Time the last frame was read

    unsigned long long mDeltaTime = 0;          //Time of the last frame

    unsigned int mFpsLimit = 60;                //Limit of FPS

    float mUpdateTime = 0;                      //Time since last FPS calculation (reset every 500000 microseconds)
    unsigned int mUpdateFrames = 0;             //Frames since last FPS calculation (reset every 500000 microseconds)

    //Debugging variables

    long long mRealTime = 0;                    //Total real time since start of Timer (in microseconds)
    long long mTotalTime = 0;                   //Time since start of Timer taking into account the timer speed (in microseconds)

    unsigned int mTotalFrames = 0;              //Total frames since start of Timer

    std::vector<float> mFpsLog;                 //Log of the last 100 FPS calculated
    std::vector<unsigned long long> mMicroLog;  //Log of the time of the last 100 frames (in microseconds)

    bool mUpdateFpsLog = false;                 //True if enough time has passed (500000 microseconds) to calculate FPS

    float mLowestFps = mFpsLimit;
    long long mLowestFpsTime = 0;

    long long mFrameDelay = 0;                  //Time in microseconds the last frame was delayed for

    long long mSlowestFrameTime = 0;
    int mSlowestFrame = 0;

    //Vsync

    bool mEnabledVsync = true;

};