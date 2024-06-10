#include "ModuleAudio.h"
#include "Globals.h"
#include "AudioSourceComponent.h"
#include "fmod_errors.h"
#include "fmod_studio.hpp"
#include "Application.h"

ModuleAudio::ModuleAudio()
{
}

ModuleAudio::~ModuleAudio()
{
}

bool ModuleAudio::Init()
{
	// Instantiate Fmod studio
	CheckError( FMOD::Studio::System::create(&mSystem) ); // Create the Studio System object.
	CheckError(mSystem->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0));

	CheckError( mSystem->getCoreSystem(&mCoreSystem));

	// Load bank
	CheckError( mSystem->loadBankFile(("Assets/FMOD/Master.strings.bank"), FMOD_STUDIO_LOAD_BANK_NORMAL, &mStringBank) );
	CheckError( mSystem->loadBankFile(("Assets/FMOD/Master.bank"), FMOD_STUDIO_LOAD_BANK_NORMAL, &mMasterBank) );
	CheckError( mSystem->loadBankFile(("Assets/FMOD/SFX.bank"), FMOD_STUDIO_LOAD_BANK_NORMAL, &mSFXBank));
	CheckError( mSystem->loadBankFile(("Assets/FMOD/Music.bank"), FMOD_STUDIO_LOAD_BANK_NORMAL, &mMusicBank));
	CheckError( mSystem->loadBankFile(("Assets/FMOD/Vehicles.bank"), FMOD_STUDIO_LOAD_BANK_NORMAL, &mVehicleBank) );

	// Set volume lower by default
	FMOD::Studio::Bus* masterBus = nullptr;
	CheckError(mSystem->getBus("bus:/", &masterBus));

	CheckError(masterBus->setVolume(0.5f));
	return true;
}

update_status ModuleAudio::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleAudio::Update(float dt)
{
	mSystem->update();

	return UPDATE_CONTINUE;
}

update_status ModuleAudio::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

std::vector<const char*> ModuleAudio::GetEventsNames()
{
	std::vector<const char*> result;
	FMOD::Studio::Bank* bankArray[EVENT_BANK_UPPERLIMIT];
	int bankCount = 0;

	mSystem->getBankList(bankArray, EVENT_BANK_UPPERLIMIT, &bankCount);
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

void ModuleAudio::AudioPause()
{
	for (auto audioSource : mAudiosSourceList) 
	{
		audioSource->PauseCurrentInstance();
	}
}

void ModuleAudio::AudioResume()
{
	for (auto audioSource : mAudiosSourceList) 
	{
		audioSource->ResumeCurrentInstance();
	}
}

void ModuleAudio::EngineStop()
{
	mAudiosSourceList = std::vector<AudioSourceComponent*>();
}

//int ModuleAudio::Play(FMOD::Studio::EventDescription* eventDescription, float3 audioPosition, const std::unordered_map<std::string, int>& parameters)
//{
//	// Create a new instance
//	FMOD::Studio::EventInstance* eventInstance = nullptr;
//	eventDescription->createInstance(&eventInstance);
//
//	// Play it
//	StartEvent(eventInstance, audioPosition);
//
//	for (const auto& param : parameters)
//	{
//		const std::string& paramName = param.first;
//		int paramValue = param.second;
//
//		ChangeEventParameter(eventInstance, paramName, paramValue);
//	}
//
//	
//	
//	CheckError(eventDescription->getInstanceCount(&count));
//
//	// Return id
//	return count;
//}

int ModuleAudio::Play(const FMOD::Studio::EventDescription* eventDescription, const int id)
{
	// Returns: -1 Continue an audio | else: play new audio 
	FMOD::Studio::EventInstance* eventInstance = nullptr;
	// Continue audio
	if (id == -1)
	{
		// Play new audio
		eventDescription->createInstance(&eventInstance);
	}
	else 
	{
		FMOD::Studio::EventInstance* eventInstance = FindEventInstance(eventDescription, id);
		eventInstance->start();
		return -1;
	}

	// Play it
	eventInstance->start();

	int count = 0;
	CheckError(eventDescription->getInstanceCount(&count));

	return count - 1;
}

void ModuleAudio::Release(const FMOD::Studio::EventDescription* eventDescription, const int id, bool fadeout)
{
	FMOD::Studio::EventInstance* eventInstance = FindEventInstance(eventDescription, id);
	
	if (eventInstance != nullptr)
	{
		if (fadeout)
		{
			eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
		}
		else
		{
			eventInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
		}
	}
	else {
		LOG("Cannot release event");
	}
}

void ModuleAudio::GetParameters(const FMOD::Studio::EventDescription* eventDescription, const int id, std::vector<int>& index, std::vector<const char*>& names, std::vector<float>& values)
{
	FMOD::Studio::EventInstance* eventInstance = FindEventInstance(eventDescription, id);
	int count = 0;
	eventDescription->getParameterDescriptionCount(&count);

	int parameterIndex = 0;
	for (auto i = 0; i < count; i++)
	{
		FMOD_STUDIO_PARAMETER_DESCRIPTION parameter;
		eventDescription->getParameterDescriptionByIndex(i, &parameter);

		float value = 0;
		eventInstance->getParameterByID(parameter.id, &value);

		index.push_back(parameterIndex);
		names.push_back(parameter.name);
		values.push_back(value);
		parameterIndex++;
	}
}

void ModuleAudio::UpdateParameter(const FMOD::Studio::EventDescription* eventDescription, const int id, const std::string& parameterName, const int parameterValue)
{
	FMOD::Studio::EventInstance* eventInstance = FindEventInstance(eventDescription, id);

	FMOD_STUDIO_PARAMETER_DESCRIPTION parameter;
	CheckError(eventDescription->getParameterDescriptionByName(parameterName.c_str(), &parameter));

	CheckError(eventInstance->setParameterByID(parameter.id, parameterValue));
}


void ModuleAudio::SetEventPosition(const FMOD::Studio::EventDescription* eventDescription, const int id ,FMOD::Studio::EventInstance* event, float3 eventPosition)
{
	FMOD::Studio::EventInstance* eventInstance = FindEventInstance(eventDescription, id);

	FMOD_3D_ATTRIBUTES attributes = { { 0 } };

	attributes.position.x = eventPosition.x;
	attributes.position.z = eventPosition.z;

	attributes.forward.z = 1.0f;
	attributes.up.y = 1.0f;

	eventInstance->set3DAttributes(&attributes);
}

void ModuleAudio::AddToAudiosList(AudioSourceComponent* audioSource)
{
	mAudiosSourceList.push_back(audioSource);
}

int ModuleAudio::GetMemoryUsage() const
{
	int currentAllocated, maxAllocated;
	CheckError( FMOD_Memory_GetStats(&currentAllocated, &maxAllocated, 1));
	// Just get memory consumed by fmod
	return currentAllocated;
}

float ModuleAudio::GetVolume(std::string busname) const
{
	FMOD::Studio::Bus* bus = nullptr;
	CheckError(mSystem->getBus(busname.c_str(), &bus));

	float volume = 0.0f;
	CheckError(bus->getVolume(&volume));

	return volume;
}

void ModuleAudio::SetVolume(std::string busname, float value) const
{
	FMOD::Studio::Bus* bus = nullptr;
	CheckError(mSystem->getBus(busname.c_str(), &bus));

	CheckError(bus->setVolume(value));
}

void ModuleAudio::CheckFmodErrorFunction(FMOD_RESULT result, const char* file, int line)
{
	if (result != FMOD_OK)
	{
		LOG("%s(%d): FMOD error %d - %s", file, line, result, FMOD_ErrorString(result));
	}
}

FMOD::Studio::EventInstance* ModuleAudio::FindEventInstance(const FMOD::Studio::EventDescription* eventDescription, const int id)
{
	int capacity = 1024;
	std::vector<FMOD::Studio::EventInstance*> instances(capacity);

	int count = 0;
	CheckError(eventDescription->getInstanceList(instances.data(), capacity, &count));

	FMOD::Studio::EventInstance* eventInstance = nullptr;
	eventInstance = instances.at(id);

	return eventInstance;
}

bool ModuleAudio::CleanUp()
{
	mSystem->unloadAll();
	mSystem->release();

	mAudiosSourceList.clear();
    return true;
}
