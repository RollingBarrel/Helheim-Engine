#pragma once

class Timer {
public:
	Timer() = default;

	void Start();
	long Read();
	long ReadDelta();
	long Stop() { SetSpeed(0); }
	long SetSpeed(float speed);

	void StartPrecise();
	long long ReadPrecise();
	long long StopPrecise();

private:
	float speed = 1;
	long mLastReadTime = 0;
	long mTotalTime = 0;

	// Precise
	long long mInitialCount = 0;
	long long mElapsedNanoseconds = 0;
};