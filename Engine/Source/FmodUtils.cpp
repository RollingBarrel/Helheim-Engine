#include "FmodUtils.h"
#include "Globals.h"
#include "fmod.hpp"
#include "fmod_errors.h"

void FmodUtils::CheckFmodErrorFunction(FMOD_RESULT result, const char* file, int line)
{
	if (result != FMOD_OK)
	{
		LOG("%s(%d): FMOD error %d - %s", file, line, result, FMOD_ErrorString(result));
	}
}
