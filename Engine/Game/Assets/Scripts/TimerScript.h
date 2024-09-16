#pragma once
class TimerScript
{
public:

	bool Delay(float time);
	bool DelayWithoutReset(float time);
	void Reset() { mTimePassed = 0.0f; }
	float GetTimePassed() { return mTimePassed; }

private:
	float mTimePassed = 0.0f;

};

