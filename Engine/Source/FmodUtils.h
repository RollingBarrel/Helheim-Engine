#pragma once
#include "Globals.h"
#include "fmod_studio.hpp"
#include <vector>

#define CheckError(result) FmodUtils::CheckFmodError(result)
#define CheckFmodError(_result) FmodUtils::CheckFmodErrorFunction(_result, __FILE__, __LINE__)

class AudioSourceComponent;

class ENGINE_API FmodUtils
{
public:
	static void CheckFmodErrorFunction(FMOD_RESULT result, const char* file, int line);

	// Events
	static std::vector<const char*> GetEventsNames();
	static FMOD::Studio::EventInstance* GetEventByName(const char* name);

	static const char* GetNameByEvent(FMOD::Studio::EventInstance*);

	// Event Parameters
	static std::vector<FMOD_STUDIO_PARAMETER_DESCRIPTION> GetParametersByEvent(FMOD::Studio::EventInstance* event);
	static void GetParametersMaxMinByComponent(AudioSourceComponent* audioSource, const char* eventName, float&max, float& min);
};