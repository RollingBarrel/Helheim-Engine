#pragma once
#include "fmod_studio.hpp"
#include <vector>

#define CheckError(result) FmodUtils::CheckFmodError(result)

class FmodUtils
{
public:
	static void CheckFmodErrorFunction(FMOD_RESULT result, const char* file, int line);
	#define CheckFmodError(_result) CheckFmodErrorFunction(_result, __FILE__, __LINE__)

	// Events
	static std::vector<const char*> GetEventsNames();
	static FMOD::Studio::EventInstance* GetEventByName(const char* name);

	static const char* GetNameByEvent(FMOD::Studio::EventInstance*);

	// Event Parameters
	static std::vector<FMOD_STUDIO_PARAMETER_DESCRIPTION> GetParametersByEvent(FMOD::Studio::EventInstance* event);
};

