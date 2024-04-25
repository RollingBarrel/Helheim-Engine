#include "FmodUtils.h"
#include "Application.h"
#include "Globals.h"
#include "fmod.hpp"
#include "fmod_errors.h"
#include "ModuleAudio.h"
#include "AudioSourceComponent.h"
#include <vector>

void FmodUtils::CheckFmodErrorFunction(FMOD_RESULT result, const char* file, int line)
{
	if (result != FMOD_OK)
	{
		LOG("%s(%d): FMOD error %d - %s", file, line, result, FMOD_ErrorString(result));
	}
}

std::vector<const char*> FmodUtils::GetEventsNames()
{
	std::vector<const char*> result;
	FMOD::Studio::System* system = App->GetAudio()->GetFMODSystem();
	FMOD::Studio::Bank* bankArray[EVENT_BANK_UPPERLIMIT];
	int bankCount = 0;

	system->getBankList(bankArray, EVENT_BANK_UPPERLIMIT, &bankCount);
	result.reserve(bankCount);
	for (auto i = 0; i < bankCount; i++) 
	{
		FMOD::Studio::Bank* bank = bankArray[i];
		FMOD::Studio::EventDescription* eventArray[EVENT_BANK_UPPERLIMIT];
		int eventDescriptionCount = 0;

		CheckError(bank->getEventList(eventArray, EVENT_BANK_UPPERLIMIT, &eventDescriptionCount));

		for (int j = 0; j < eventDescriptionCount; ++j) 
		{

			// Name to string
			const int bufferSize = 256;

			char pathBuffer[bufferSize];
			int retrievedSize = 0;
			CheckError(eventArray[j]->getPath(pathBuffer, bufferSize, &retrievedSize));

			char* copyBuffer = new char[bufferSize];
			strcpy_s(copyBuffer, bufferSize, pathBuffer);

			result.push_back(copyBuffer);
		}
	}

	return result;
}

FMOD::Studio::EventInstance* FmodUtils::GetEventByName(const char* name)
{
	FMOD::Studio::System* system = App->GetAudio()->GetFMODSystem();
	FMOD::Studio::EventDescription* eventDescription = nullptr;
	CheckError(system->getEvent(name, &eventDescription));

	FMOD::Studio::EventInstance* event = nullptr;
	eventDescription->createInstance(&event);

	return event;
}

const char* FmodUtils::GetNameByEvent(FMOD::Studio::EventInstance*)
{
	return nullptr;
}

std::vector<FMOD_STUDIO_PARAMETER_DESCRIPTION> FmodUtils::GetParametersByEvent(FMOD::Studio::EventInstance* event)
{

	return std::vector<FMOD_STUDIO_PARAMETER_DESCRIPTION>();
}

void FmodUtils::GetParametersMaxMinByComponent(AudioSourceComponent* audioSource, const char* eventName, float& max, float& min)
{
	FMOD_STUDIO_PARAMETER_DESCRIPTION paramDesc;
	CheckError(audioSource->GetEventDescription()->getParameterDescriptionByName(eventName, &paramDesc));

	max = paramDesc.maximum;
	min = paramDesc.minimum;
}


