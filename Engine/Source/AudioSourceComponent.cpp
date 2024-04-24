#include "AudioSourceComponent.h"
#include "FmodUtils.h"

#include "GameObject.h"
#include "Application.h"

#include "fmod_studio.hpp"
#define CheckError(result) FmodUtils::CheckFmodError(result)

AudioSourceComponent::AudioSourceComponent(GameObject* ownerGameObject): Component(ownerGameObject,ComponentType::AUDIOSOURCE)
{
}

AudioSourceComponent::AudioSourceComponent(const AudioSourceComponent& original, GameObject* owner)
	: Component(owner, ComponentType::AUDIOSOURCE)
{
	mName = original.GetName();
	SetEventByName(mName.c_str());
	mParameters = original.GetParameters();
}

AudioSourceComponent::~AudioSourceComponent()
{
	Reset();
	mEventInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
	mEventInstance->release();
}


void AudioSourceComponent::GetParametersNameAndValue(std::vector<int>& index, std::vector<const char*>& names, std::vector<float>& values)
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

void AudioSourceComponent::SetEventInstance(FMOD::Studio::EventInstance* event)
{
	mEventInstance = event;

	if (mEventInstance != nullptr) 
	{

		// Update parameters
		event->getDescription(&mEventDescription);
		UpdateParameters();
	}
}

void AudioSourceComponent::SetEventByName(const char* eventName)
{
	SetEventInstance(FmodUtils::GetEventByName(eventName));
	size_t eventNameLength = strlen(eventName);

	mName = eventName;
}

void AudioSourceComponent::UpdateParameterValueByIndex(int index, float value)
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

void AudioSourceComponent::UpdateParameterValueByName(const char* name, float value)
{
	mEventInstance->setParameterByName(name, value);
}

void AudioSourceComponent::SmoothUpdateParameterValueByName(const char* name, float targetValue, float transitionTime)
{
	auto it = mNameToParameters.find(name);
	if (it != mNameToParameters.end())
	{
		int index = it->second;
		float value = GetParameterValueByIndex(index);

		float step = (targetValue - value) / transitionTime;
		float newValue = value + step * App->GetGameDt();
		UpdateParameterValueByIndex(index, newValue);
	}
}

void AudioSourceComponent::Update()
{
	// UPDATE 3D parameters
	float3 gameobjectPosition = GetOwner()->GetPosition();

	FMOD_3D_ATTRIBUTES attributes = { { 0 } };

	attributes.position.x = gameobjectPosition.x;
	attributes.position.z = gameobjectPosition.z;

	attributes.forward.z = 1.0f;
	attributes.up.y = 1.0f;

	// Play Audio
	mEventInstance->set3DAttributes(&attributes);
}

void AudioSourceComponent::Play()
{
	mEventInstance->start();
}

void AudioSourceComponent::PlayOneShot()
{
	FMOD::Studio::EventInstance* eventInstance = NULL;
	mEventDescription->createInstance(&eventInstance);

	eventInstance->start();
	eventInstance->release();
}

void AudioSourceComponent::Stop(bool fadeout)
{
	if (fadeout) {
		mEventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
	}
	else {
		mEventInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
	}
}

Component* AudioSourceComponent::Clone(GameObject* owner) const
{
	return new AudioSourceComponent(*this, owner);
}

void AudioSourceComponent::Save(Archive& archive) const
{
	archive.AddInt("ComponentType", static_cast<int>(GetType()));

	archive.AddString("EventName", mName.c_str());

	std::vector<int> parameterKeys;
	std::vector<float> parameterValues;

	for (const auto& pair : mParameters) 
	{
		parameterKeys.push_back(pair.first);
		parameterValues.push_back(pair.second);
	}

	std::vector<Archive> objectArray;
	for (auto i = 0; i < parameterKeys.size(); i++)
	{
		Archive dataArchive;
		dataArchive.AddInt("ParametersKey", parameterKeys[i]);
		dataArchive.AddFloat("ParametersValue", parameterValues[i]);

		objectArray.push_back(dataArchive);
	}

	archive.AddObjectArray("ParametersVariables", objectArray);
}

void AudioSourceComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
	if (data.HasMember("EventName") && data["EventName"].IsString()) 
	{
		SetEventByName(data["EventName"].GetString());
	}

	if (data.HasMember("ParametersVariables") && data["ParametersVariables"].IsArray()) 
	{
		UpdateParameters();

		const auto& array = data["ParametersVariables"].GetArray();

		for (unsigned int i = 0; i < array.Size(); ++i)
		{
			unsigned key = array[i]["ParametersKey"].GetInt();
			int value = array[i]["ParametersValue"].GetFloat();

			UpdateParameterValueByIndex(key, value);
		}
	}
}

void AudioSourceComponent::Enable()
{
	Play();
}

void AudioSourceComponent::Disable()
{
	Stop(false);
}

void AudioSourceComponent::Reset()
{
	mParameters.clear();
}

float AudioSourceComponent::GetParameterValueByIndex(int index)
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

void AudioSourceComponent::UpdateParameters()
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
