#pragma once

class PreciseTimer {
public:
	PreciseTimer() = default;

	void Start();
	long long Read();
	long long ReadDelta();
	long long Stop();


private:

	long long mInitialCount = 0;
	long long mLastCount = 0;
	long long mTotalTime = 0;
};