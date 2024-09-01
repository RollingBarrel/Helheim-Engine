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
#if _DEBUG
	CheckError(mSystem->initialize(1024, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_NORMAL, 0));
#else
	CheckError(mSystem->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0));
#endif

	CheckError( mSystem->getCoreSystem(&mCoreSystem));

	// Load bank
	CheckError( mSystem->loadBankFile(("Assets/FMOD/Desktop/Master.strings.bank"), FMOD_STUDIO_LOAD_BANK_NORMAL, &mStringBank) );
	CheckError( mSystem->loadBankFile(("Assets/FMOD/Desktop/Master.bank"), FMOD_STUDIO_LOAD_BANK_NORMAL, &mMasterBank) );
	CheckError( mSystem->loadBankFile(("Assets/FMOD/Desktop/sfx.bank"), FMOD_STUDIO_LOAD_BANK_NORMAL, &mSFXBank));
	CheckError( mSystem->loadBankFile(("Assets/FMOD/Desktop/music.bank"), FMOD_STUDIO_LOAD_BANK_NORMAL, &mMusicBank));
	CheckError( mSystem->loadBankFile(("Assets/FMOD/Desktop/amb.bank"), FMOD_STUDIO_LOAD_BANK_NORMAL, &mAmbBank));
	CheckError(mSystem->loadBankFile(("Assets/FMOD/Desktop/ui.bank"), FMOD_STUDIO_LOAD_BANK_NORMAL, &mUiBank));

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
	for (const auto& eventDescription : mActiveEvent)
	{
		int capacity = 1024;
		std::vector<FMOD::Studio::EventInstance*> instances(capacity);

		int count = 0;
		CheckError(eventDescription->getInstanceList(instances.data(), capacity, &count));

		for (const auto& instance : instances)
		{
			instance->setPaused(true);
		}
	}
}

void ModuleAudio::AudioResume()
{
	for (const auto& eventDescription : mActiveEvent)
	{
		int capacity = 1024;
		std::vector<FMOD::Studio::EventInstance*> instances(capacity);

		int count = 0;
		CheckError(eventDescription->getInstanceList(instances.data(), capacity, &count));

		for (const auto& instance : instances)
		{
			instance->setPaused(false);

		}
	}
}

void ModuleAudio::EngineStop()
{
	for (const auto& eventDescription : mActiveEvent)
	{
		int capacity = 1024;
		std::vector<FMOD::Studio::EventInstance*> instances(capacity);

		int count = 0;
		CheckError(eventDescription->getInstanceList(instances.data(), capacity, &count));

		for (const auto& instance : instances)
		{
			instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
			instance->release();
		}
	}
}

int ModuleAudio::Play(const FMOD::Studio::EventDescription* eventDescription, const int id)
{
	AddIntoEventList(eventDescription);
	// Returns: -1 Continue an audio | else: play new audio 
	FMOD::Studio::EventInstance* eventInstance = nullptr;
	// Restart audio
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

void ModuleAudio::Pause(const FMOD::Studio::EventDescription* eventDescription, const int id, bool pause)
{
	FMOD::Studio::EventInstance* eventInstance = FindEventInstance(eventDescription, id);
	
	if (eventInstance != nullptr)
	{
		eventInstance->setPaused(pause);
	}
	else 
	{
		LOG("Cannot stop event");
	}
}

void ModuleAudio::Stop(const FMOD::Studio::EventDescription* eventDescription, const int id)
{
	FMOD::Studio::EventInstance* eventInstance = FindEventInstance(eventDescription, id);

	if (eventInstance != nullptr)
	{
		eventInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
	}
	else 
	{
		LOG("Cannot release event");
	}
}

void ModuleAudio::Release(const FMOD::Studio::EventDescription* eventDescription, const int id)
{
	FMOD::Studio::EventInstance* eventInstance = FindEventInstance(eventDescription, id);

	if (eventInstance != nullptr)
	{
		eventInstance->release();
	}
	else 
	{
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

void ModuleAudio::UpdateParameter(const FMOD::Studio::EventDescription* eventDescription, const int id, const std::string& parameterName, const float parameterValue)
{
	FMOD::Studio::EventInstance* eventInstance = FindEventInstance(eventDescription, id);

	FMOD_STUDIO_PARAMETER_DESCRIPTION parameter;
	CheckError(eventDescription->getParameterDescriptionByName(parameterName.c_str(), &parameter));

	CheckError(eventInstance->setParameterByID(parameter.id, parameterValue));
}

void ModuleAudio::SetEventPosition(const FMOD::Studio::EventDescription* eventDescription, const int id, float3 eventPosition)
{
	FMOD::Studio::EventInstance* eventInstance = FindEventInstance(eventDescription, id);

	FMOD_3D_ATTRIBUTES attributes = { { 0 } };

	attributes.position.x = eventPosition.x;
	attributes.position.z = eventPosition.z;

	attributes.forward.z = 1.0f;
	attributes.up.y = 1.0f;

	eventInstance->set3DAttributes(&attributes);
}

int ModuleAudio::GetMemoryUsage() const
{
	int currentAllocated, maxAllocated;
	CheckError( FMOD_Memory_GetStats(&currentAllocated, &maxAllocated, 1));
	// Just get memory consumed by fmod
	return currentAllocated;
}

void ModuleAudio::GetInstances(std::map<std::string, int>& instances) const
{
	instances.clear();

	for (const auto& eventDescription : mActiveEvent)
	{
		// Get the path of the event description
		char path[256];
		int retrieved = 0;
		CheckError(eventDescription->getPath(path, sizeof(path), &retrieved));

		// Get the instance count of the event description
		int instanceCount = 0;
		CheckError(eventDescription->getInstanceCount(&instanceCount));

		// Store the path and instance count in the map
		instances[std::string(path)] = instanceCount - 1;
	}
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

void ModuleAudio::AddIntoEventList(const FMOD::Studio::EventDescription* eventDescription)
{
	auto it = std::find(mActiveEvent.begin(), mActiveEvent.end(), eventDescription);

	// If not found, add it to the list
	if (it == mActiveEvent.end())
	{
		mActiveEvent.push_back(const_cast<FMOD::Studio::EventDescription*>(eventDescription));
	}

}

bool ModuleAudio::CleanUp()
{
	mSystem->unloadAll();
	mSystem->release();

    return true;
}
