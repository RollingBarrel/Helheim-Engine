#include "AudioSourceComponent.h"
#include "ModuleAudio.h"

#include "GameObject.h"
#include "Application.h"

#include "fmod_studio.hpp"

#define CheckError(result) ModuleAudio::CheckFmodError(result)

AudioSourceComponent::AudioSourceComponent(GameObject* ownerGameObject): Component(ownerGameObject,ComponentType::AUDIOSOURCE)
{
	App->GetAudio()->AddToAudiosList(this);
}

AudioSourceComponent::AudioSourceComponent(const AudioSourceComponent& original, GameObject* owner)
	: Component(owner, ComponentType::AUDIOSOURCE)
{
	mName = original.GetName();
	SetEventByName(mName.c_str());
	mParameters = original.GetParameters();

	App->GetAudio()->AddToAudiosList(this);
}

AudioSourceComponent::~AudioSourceComponent()
{
	CleanCurrentInstance();
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
	FMOD::Studio::System* system = App->GetAudio()->GetFMODSystem();
	FMOD::Studio::EventDescription* eventDescription = nullptr;
	system->getEvent(eventName, &eventDescription);

	FMOD::Studio::EventInstance* event = nullptr;
	eventDescription->createInstance(&event);

	SetEventInstance(event);
	size_t eventNameLength = strlen(eventName);

	mName = eventName;
}

void AudioSourceComponent::GetParametersMaxMin(const char* eventName, float& max, float& min)
{
	FMOD_STUDIO_PARAMETER_DESCRIPTION paramDesc;
	CheckError(mEventDescription->getParameterDescriptionByName(eventName, &paramDesc));

	max = paramDesc.maximum;
	min = paramDesc.minimum;
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
		float newValue = value + step * App->GetDt();
		UpdateParameterValueByIndex(index, newValue);
	}
}

void AudioSourceComponent::UpdatePosition(float3 position)
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

void AudioSourceComponent::Update()
{
	// UPDATE 3D parameters
	if (mPositionWithGameObject)
	{
		float3 gameobjectPosition = GetOwner()->GetWorldPosition();

		FMOD_3D_ATTRIBUTES attributes = { { 0 } };

		attributes.position.x = gameobjectPosition.x;
		attributes.position.z = gameobjectPosition.z;

		attributes.forward.z = 1.0f;
		attributes.up.y = 1.0f;

		mEventInstance->set3DAttributes(&attributes);
	}
}

void AudioSourceComponent::Play()
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

void AudioSourceComponent::PlayWithVolume(float volume)
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

void AudioSourceComponent::PlayOneShot()
{
	if (this != nullptr)
	{
		FMOD::Studio::EventInstance* eventInstance = nullptr;
		mEventDescription->createInstance(&eventInstance);

		eventInstance->start();
		float3 gameobjectPosition = GetOwner()->GetWorldPosition();

		FMOD_3D_ATTRIBUTES attributes = { { 0 } };

		attributes.position.x = gameobjectPosition.x;
		attributes.position.z = gameobjectPosition.z;

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

void AudioSourceComponent::PlayOneShotPosition(float3 position)
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

void AudioSourceComponent::Stop(bool fadeout)
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

Component* AudioSourceComponent::Clone(GameObject* owner) const
{
	return new AudioSourceComponent(*this, owner);
}

void AudioSourceComponent::Save(JsonObject& obj) const
{
	Component::Save(obj);
	obj.AddString("EventName", mName.c_str());

	JsonArray arr = obj.AddNewJsonArray("ParametersVariables");
	for (const auto& pair : mParameters) 
	{
		JsonObject data = arr.PushBackNewObject();
		data.AddInt("ParametersKey", pair.first);
		data.AddFloat("ParametersValue", pair.second);
	}
}

void AudioSourceComponent::Load(const JsonObject& data)
{
	Component::Load(data);

	SetEventByName(data.GetString("EventName").c_str());

	UpdateParameters();

	JsonArray arr = data.GetJsonArray("ParametersVariables");
	for (unsigned int i = 0; i < arr.Size(); ++i)
	{
		UpdateParameterValueByIndex(arr.GetInt(i), arr.GetFloat(i+1));
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

void AudioSourceComponent::PauseCurrentInstance()
{
	bool isPause;
	mEventInstance->getPaused(&isPause);

	if (isPause != true)
	{
		mEventInstance->setPaused(true);
	}
}

void AudioSourceComponent::ResumeCurrentInstance()
{
	bool isPause;
	mEventInstance->getPaused(&isPause);

	if (isPause == true)
	{
		mEventInstance->setPaused(false);
	}
}

void AudioSourceComponent::CleanCurrentInstance()
{
	Reset();
	mEventInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
	mEventInstance->release();
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
