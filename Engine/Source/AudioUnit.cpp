#include "AudioUnit.h"
#include "ModuleAudio.h"

#include "GameObject.h"
#include "Application.h"

#include "fmod_studio.hpp"

#define CheckError(result) ModuleAudio::CheckFmodError(result)

AudioUnit::AudioUnit()
{
}

AudioUnit::~AudioUnit()
{
	CleanCurrentInstance();
}


void AudioUnit::GetParametersNameAndValue(std::vector<int>& index, std::vector<const char*>& names, std::vector<float>& values)
{
	for (const auto& pair : mParameters)
	{
		FMOD_STUDIO_PARAMETER_DESCRIPTION parameter;
		mEventDescription->getParameterDescriptionByIndex(pair.first, &parameter);

		index.push_back(pair.first);
		names.push_back(parameter.name);
		values.push_back(pair.second);
	}
}

void AudioUnit::SetEventInstance(FMOD::Studio::EventInstance* event)
{
	mEventInstance = event;

	if (mEventInstance != nullptr)
	{

		// Update parameters
		event->getDescription(&mEventDescription);
		UpdateParameters();
	}
}

void AudioUnit::SetEventByName(const char* eventName)
{
	FMOD::Studio::System* system = App->GetAudio()->GetFMODSystem();
	FMOD::Studio::EventDescription* eventDescription = nullptr;
	system->getEvent(eventName, &eventDescription);

	FMOD::Studio::EventInstance* event = nullptr;
	eventDescription->createInstance(&event);

	SetEventInstance(event);
	size_t eventNameLength = strlen(eventName);

	mName = eventName;
}

void AudioUnit::GetParametersMaxMin(const char* eventName, float& max, float& min)
{
	FMOD_STUDIO_PARAMETER_DESCRIPTION paramDesc;
	CheckError(mEventDescription->getParameterDescriptionByName(eventName, &paramDesc));

	max = paramDesc.maximum;
	min = paramDesc.minimum;
}

void AudioUnit::UpdateParameterValueByIndex(int index, float value)
{
	FMOD_STUDIO_PARAMETER_DESCRIPTION parameter;
	mEventDescription->getParameterDescriptionByIndex(index, &parameter);

	CheckError(mEventInstance->setParameterByID(parameter.id, value));

	auto it = mParameters.find(index);
	if (it != mParameters.end())
	{
		it->second = value;
	}
}

void AudioUnit::UpdateParameterValueByName(const char* name, float value)
{
	mEventInstance->setParameterByName(name, value);
}

void AudioUnit::SmoothUpdateParameterValueByName(const char* name, float targetValue, float transitionTime)
{
	auto it = mNameToParameters.find(name);
	if (it != mNameToParameters.end())
	{
		int index = it->second;
		float value = GetParameterValueByIndex(index);

		float step = (targetValue - value) / transitionTime;
		float newValue = value + step * App->GetDt();
		UpdateParameterValueByIndex(index, newValue);
	}
}

void AudioUnit::UpdatePosition(float3 position)
{
	if (!mPositionWithGameObject)
	{
		FMOD_3D_ATTRIBUTES attributes = { { 0 } };

		attributes.position.x = position.x;
		attributes.position.z = position.z;

		attributes.forward.z = 1.0f;
		attributes.up.y = 1.0f;

		mEventInstance->set3DAttributes(&attributes);
	}
}

void AudioUnit::Play()
{
	if (this != nullptr)
	{
		mEventInstance->start();
	}
	else
	{
		LOG("Cannot found audio source");
	}
}

void AudioUnit::PlayWithVolume(float volume)
{
	if (this != nullptr)
	{
		mEventInstance->setVolume(volume);
		mEventInstance->start();
	}
	else
	{
		LOG("Cannot found audio source");
	}
}

void AudioUnit::PlayOneShotPosition(float3 position)
{
	if (this != nullptr)
	{
		FMOD::Studio::EventInstance* eventInstance = nullptr;
		mEventDescription->createInstance(&eventInstance);

		eventInstance->start();

		FMOD_3D_ATTRIBUTES attributes = { { 0 } };

		attributes.position.x = position.x;
		attributes.position.z = position.z;

		attributes.forward.z = 1.0f;
		attributes.up.y = 1.0f;

		eventInstance->set3DAttributes(&attributes);
		eventInstance->release();
	}
	else
	{
		LOG("Cannot found audio source");
	}

}

void AudioUnit::Stop(bool fadeout)
{
	if (fadeout)
	{
		mEventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
	}
	else
	{
		mEventInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
	}
}


void AudioUnit::Reset()
{
	mParameters.clear();
}

float AudioUnit::GetParameterValueByIndex(int index)
{
	auto it = mParameters.find(index);
	if (it != mParameters.end())
	{
		return it->second;
	}

	else
	{
		return -1;
	}
}

void AudioUnit::PauseCurrentInstance()
{
	bool isPause;
	mEventInstance->getPaused(&isPause);

	if (isPause != true)
	{
		mEventInstance->setPaused(true);
	}
}

void AudioUnit::ResumeCurrentInstance()
{
	bool isPause;
	mEventInstance->getPaused(&isPause);

	if (isPause == true)
	{
		mEventInstance->setPaused(false);
	}
}

void AudioUnit::CleanCurrentInstance()
{
	Reset();
	mEventInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
	mEventInstance->release();
}

void AudioUnit::UpdateParameters()
{
	mParameters.clear();
	int count = 0;
	mEventDescription->getParameterDescriptionCount(&count);

	for (auto i = 0; i < count; i++)
	{
		FMOD_STUDIO_PARAMETER_DESCRIPTION parameter;
		mEventDescription->getParameterDescriptionByIndex(i, &parameter);

		mParameters.insert(std::make_pair(i, parameter.defaultvalue));
		mNameToParameters.insert(std::make_pair(parameter.name, i));
	}

}
