#include "TimerScript.h"
#include "Application.h"

bool TimerScript::Delay(float time)
{
	mTimePassed += App->GetDt();

	if (mTimePassed >= time)
	{
		mTimePassed = 0;
		return true;
	}
	else return false;
}
