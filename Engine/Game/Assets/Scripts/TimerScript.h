#pragma once
class TimerScript
{
public:

	bool Delay(float time);
	bool DelayWithoutReset(float time);
	void Reset() { mTimePassed = 0.0f; }

private:
	float mTimePassed = 0.0f;

};

