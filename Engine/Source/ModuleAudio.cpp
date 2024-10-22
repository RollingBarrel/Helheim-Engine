#include "ModuleAudio.h"
#include "AudioSourceComponent.h"

#include "Globals.h"
#include "AudioSourceComponent.h"
#include "fmod_errors.h"
#include "fmod_studio.hpp"
#include "Application.h"

FMOD_RESULT F_CALLBACK ChannelEndCallback(FMOD_CHANNELCONTROL* channelControl, FMOD_CHANNELCONTROL_TYPE controlType, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, void* commandData1, void* commandData2)
{
	if (callbackType == FMOD_CHANNELCONTROL_CALLBACK_END)
	{
		FMOD::Channel* channel = (FMOD::Channel*)channelControl;
		FMOD::Sound* sound = nullptr;
		channel->getCurrentSound(&sound);

		if (sound)
		{
			CheckError(channel->stop());
			CheckError(sound->release());  // Release the sound when playback ends
		}
	}

	return FMOD_OK;
}

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

	CheckError(mCoreSystem->createChannelGroup("CustomOneShotAudio", &mOneShotChannelGroup));
	CheckError(mCoreSystem->createChannelGroup("CustomAudio", &mAudioChannelGroup));

	CheckError(mOneShotChannelGroup->setVolume(0.5f));
	CheckError(mAudioChannelGroup->setVolume(0.5f));

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
	UpdatePendingReleases();
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

		for (int j = 0; j < count; ++j)
		{
			auto instance = instances[j];
			CheckError(instance->stop(FMOD_STUDIO_STOP_IMMEDIATE));
			CheckError(instance->release());
		}
	}

	if (mOneShotChannelGroup) 
	{
		int numChannels = 0;
		mOneShotChannelGroup->getNumChannels(&numChannels);
		for (int i = 0; i < numChannels; ++i) 
		{
			FMOD::Channel* channel = nullptr;
			if (mOneShotChannelGroup->getChannel(i, &channel) == FMOD_OK && channel) 
			{
				FMOD::Sound* sound = nullptr;
				if (channel->getCurrentSound(&sound) == FMOD_OK && sound) 
				{
					CheckError(channel->stop());
					CheckError(sound->release());
				}
			}
		}
	}

	if (mAudioChannelGroup) 
	{
		int numChannels = 0;
		mAudioChannelGroup->getNumChannels(&numChannels);
		for (int i = 0; i < numChannels; ++i) 
		{
			FMOD::Channel* channel = nullptr;
			if (mAudioChannelGroup->getChannel(i, &channel) == FMOD_OK && channel) 
			{
				FMOD::Sound* sound = nullptr;
				if (channel->getCurrentSound(&sound) == FMOD_OK && sound) 
				{
					CheckError(channel->stop());
					CheckError(sound->release());
				}
			}
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

void ModuleAudio::Restart(const FMOD::Studio::EventDescription* eventDescription, const int id)
{
	if (id != -1)
	{
		FMOD::Studio::EventInstance* eventInstance = FindEventInstance(eventDescription, id);
		eventInstance->start();
	}
}

void ModuleAudio::Pause(const FMOD::Studio::EventDescription* eventDescription, const int id, bool pause)
{
	FMOD::Studio::EventInstance* eventInstance = FindEventInstance(eventDescription, id);
	
	int count = 0;
	CheckError(eventDescription->getInstanceCount(&count));

	if (eventInstance != nullptr)
	{
		CheckError(eventInstance->setPaused(pause));
	}
	else 
	{
		LOG("Cannot stop event");
	}
}

FMOD::Channel* ModuleAudio::Play(const std::string& fileName)
{
	FMOD::Sound* sound = nullptr;
	FMOD::Channel* channel = nullptr;

	FMOD_RESULT result = mCoreSystem->createStream(fileName.c_str(), FMOD_DEFAULT, nullptr, &sound);
	CheckError(result);

	// Play the sound on a new channel
	result = mCoreSystem->playSound(sound, nullptr, false, &channel);
	CheckError(result);
	sound->set3DMinMaxDistance(1.0f, 30.0f);
	channel->setMode(FMOD_LOOP_NORMAL);
	channel->setChannelGroup(mAudioChannelGroup);
	return channel;
}

void ModuleAudio::Pause(FMOD::Channel* channel, bool state)
{
	channel->setPaused(state);
}

FMOD::Channel* ModuleAudio::PlayOneShot(const std::string& fileName)
{
	FMOD::Sound* sound = nullptr;
	FMOD::Channel* channel = nullptr; 

	// Create the sound using the FMOD system
	FMOD_RESULT result = mCoreSystem->createStream(fileName.c_str(), FMOD_DEFAULT, nullptr, &sound);
	CheckError(result);
	sound->set3DMinMaxDistance(5.0f, 30.0f);

	// Play the sound on a new channel
	result = mCoreSystem->playSound(sound, nullptr, false, &channel);

	CheckError(result);
	channel->setChannelGroup(mOneShotChannelGroup);
	return channel;
}

void ModuleAudio::Release(FMOD::Channel* channel)
{
	FMOD::Sound* sound = nullptr;
	channel->getCurrentSound(&sound);

	if (sound)
	{
		channel->stop();
		sound->release();
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
		FMOD_STUDIO_PLAYBACK_STATE state;
		eventInstance->getPlaybackState(&state);

		char eventPath[256];
		eventDescription->getPath(eventPath, sizeof(eventPath), nullptr);

		if (state == FMOD_STUDIO_PLAYBACK_STOPPED)
		{
			eventInstance->release();
			LOG("Released event: %s", eventPath); 
		}
		else
		{
			LOG("Event '%s' is not stopped, cannot release", eventPath);
			eventsPendingRelease.push_back(eventInstance);
		}
	}
	else
	{
		LOG("Cannot release event, eventInstance not found");
	}
}


void ModuleAudio::UpdatePendingReleases()
{
	for (auto it = eventsPendingRelease.begin(); it != eventsPendingRelease.end();)
	{
		FMOD_STUDIO_PLAYBACK_STATE state;
		(*it)->getPlaybackState(&state);

		if (state == FMOD_STUDIO_PLAYBACK_STOPPED)
		{
			FMOD::Studio::EventDescription* eventDescription = nullptr;
			(*it)->getDescription(&eventDescription);

			char eventPath[256];
			if (eventDescription != nullptr)
			{
				eventDescription->getPath(eventPath, sizeof(eventPath), nullptr);
			}
			else
			{
				snprintf(eventPath, sizeof(eventPath), "Unknown event");
			}

			(*it)->release();
			LOG("Released event: %s", eventPath);

			it = eventsPendingRelease.erase(it);
		}
		else
		{
			++it;
		}
	}
}


void ModuleAudio::ReleaseAllAudio()
{
	for (FMOD::Studio::EventDescription* eventDescription : mActiveEvent) 
	{
		if (eventDescription) 
		{
			int instanceCount = 0;
			eventDescription->getInstanceCount(&instanceCount);

			if (instanceCount > 0) 
			{
				std::vector<FMOD::Studio::EventInstance*> instances(instanceCount);
				eventDescription->getInstanceList(instances.data(), instanceCount, &instanceCount);

				for (FMOD::Studio::EventInstance* instance : instances) 
				{
					if (instance) 
					{
						FMOD_STUDIO_PLAYBACK_STATE state;
						instance->getPlaybackState(&state);

						if (state == FMOD_STUDIO_PLAYBACK_STOPPED)
						{
							instance->release();
						}
						else
						{
							eventsPendingRelease.push_back(instance);
						}
					}
				}
			}
		}
	}

	mActiveEvent.clear();
	//mOneShotChannelGroup->release();
	//mAudioChannelGroup->release();
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

void ModuleAudio::SetAudioPosition(FMOD::Channel* channel, float3 audioPosition)
{
	if(audioPosition.Distance(float3(0,0,0)) == 0)
	{
		return;
	}

	// Add the channel to the global channel group
	if (channel)
	{
		channel->setMode(FMOD_3D);

		FMOD::Sound* sound = nullptr;

		FMOD_3D_ATTRIBUTES attributes = { { 0 } };

		attributes.position.x = audioPosition.x;
		attributes.position.z = audioPosition.z;

		attributes.forward.z = 1.0f;
		attributes.up.y = 1.0f;

		channel->set3DAttributes(&attributes.position, &attributes.velocity);
	}
}

void ModuleAudio::SetLoop(FMOD::Channel* channel, bool loop)
{
	if (loop)
	{
		CheckError(channel->setMode(FMOD_LOOP_NORMAL));
	}
	else
	{
		CheckError(channel->setMode(FMOD_LOOP_OFF));
	}
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

	// We can remove this +0.1 if it feels unbalances

	if (busname.compare("bus:/music") == 0)
	{
		float finalVolume = (value + 0.1f) * GetVolume("bus:/");

		CheckError(mAudioChannelGroup->setVolume(finalVolume));
	}

	if (busname.compare("bus:/sfx") == 0)
	{
		float finalVolume = (value + 0.1f) * GetVolume("bus:/");

		CheckError(mOneShotChannelGroup->setVolume(finalVolume));
	}

	if (busname.compare("bus:/") == 0)
	{
		float oneShotfinalVolume = value * GetVolume("bus:/sfx");
		float AudiofinalVolume = value * GetVolume("bus:/music");

		CheckError(mOneShotChannelGroup->setVolume(oneShotfinalVolume));
		CheckError(mAudioChannelGroup->setVolume(AudiofinalVolume));
	}
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
	// Unload all loaded banks
	if (mUiBank)
	{
		mUiBank->unload();
		mUiBank = nullptr;
	}
	if (mAmbBank)
	{
		mAmbBank->unload();
		mAmbBank = nullptr;
	}
	if (mMusicBank)
	{
		mMusicBank->unload();
		mMusicBank = nullptr;
	}
	if (mSFXBank)
	{
		mSFXBank->unload();
		mSFXBank = nullptr;
	}
	if (mMasterBank)
	{
		mMasterBank->unload();
		mMasterBank = nullptr;
	}
	if (mStringBank)
	{
		mStringBank->unload();
		mStringBank = nullptr;
	}

	// Release channel groups
	if (mOneShotChannelGroup)
	{
		mOneShotChannelGroup->release();
		mOneShotChannelGroup = nullptr;
	}
	if (mAudioChannelGroup)
	{
		mAudioChannelGroup->release();
		mAudioChannelGroup = nullptr;
	}

	// Release the Studio System and Core System
	if (mSystem)
	{
		// Release the Studio System object
		mSystem->release();
		mSystem = nullptr;
	}

	if (mCoreSystem)
	{
		// Release the Studio System object
		mCoreSystem->release();
		mCoreSystem = nullptr;
	}


    return true;
}
