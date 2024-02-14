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

    long Stop();

    long SetSpeed(float speed);
    void SetTimeScale(float speed);

    float GetSpeed() const { return mSpeed; }
    float GetNewSpeed() const { return mNewSpeed; }

    long GetDelta() const { return mDeltaTime; }

    float GetFPS() const
    {
        if (!mFpsLog.empty())
        {
            return mFpsLog.back();
        }
        return 0;
    }

    unsigned long GetRealTime() const { return mRealTime; }
    unsigned long GetTotalTime() const { return mTotalTime; }
    unsigned int GetTotalFrames() const { return mTotalFrames; }

    unsigned int GetFpsLimit() const { return mFpsLimit; }
    void SetFpsLimit(unsigned int limit) { mFpsLimit = limit; }

    std::vector<float> GetFpsLog() const { return mFpsLog; }
    std::vector<unsigned long> GetMsLog() const { return mMsLog; }

    bool UpdateFpsLog() const { return mUpdateFpsLog; }
    void FpsLogUpdated() { mUpdateFpsLog = false; }

    unsigned int GetLowestFPS() const { return mLowestFps; }
    long GetLowestFpsTime() const { return mLowestFpsTime; }
    void SetLowestFps();

    long GetFrameDelay() const { return mFrameDelay; }

    long GetSlowestFrameTime() const { return mSlowestFrameTime; }
    int GetSlowestFrame() const { return mSlowestFrame; }
    void SetSlowestFrame();



private:
	float mSpeed = 1.0f;
    float mNewSpeed = 1.0f;

	long mLastReadTime = 0;         //Time the last frame was read

    unsigned long mDeltaTime = 0;   //Time of the last frame

    unsigned int mFpsLimit = 60;    //Limit of FPS

    float mUpdateTime = 0;           //Time since last FPS calculation (reset every 500 ms)
    unsigned int mUpdateFrames = 0; //Frames since last FPS calculation (reset every 500 ms)

    bool mChangeSpeed = false;

    //Debugging variables

    long mRealTime = 0;            //Total real time since start of Timer
    long mTotalTime = 0;             //Time since start of Timer taking into account the timer speed

    unsigned int mTotalFrames = 0;  //Total frames since start of Timer

    std::vector<float> mFpsLog;         //Log of the last 100 FPS calculated
    std::vector<unsigned long> mMsLog;  //Log of the time of the last 100 frames

    bool mUpdateFpsLog = false;         //True if enough time has passed (500ms) to calculate FPS

    unsigned int mLowestFps = mFpsLimit;
    long mLowestFpsTime = 0;

    long mFrameDelay = 0;               //Time in ms the last frame was delayed for

    long mSlowestFrameTime = 0;
    int mSlowestFrame = 0;

};