#pragma once

class Timer {
public:
	Timer() = default;

	void Start();
	void StartWithRunTime();
	long Read();
	long ReadDelta();
	long Stop() { SetSpeed(0); }
	long SetSpeed(float speed);

private:
	float mSpeed = 1;
	long mLastReadTime = 0;
	long mTotalTime = 0;
};