#include "Timer.h"
#include "Application.h"

bool Timer::Delay(float time)
{
	mTimePassed += App->GetDt();

	if (mTimePassed >= time)
	{
		mTimePassed = 0;
		return true;
	}
	else return false;
}
