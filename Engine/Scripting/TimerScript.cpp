#include "TimerScript.h"
#include "Application.h"

bool TimerScript::Delay(float time)
{
	mTimePassed += App->GetDt();

	if (mTimePassed >= time)
	{
		Reset();
		return true;
	}
	else return false;
}

bool TimerScript::DelayWithoutReset(float time)
{
	mTimePassed += App->GetDt();

	if (mTimePassed >= time)
	{
		return true;
	}
	else return false;
}
