#include "AudioSourceComponent.h"
#include "FmodUtils.h"

AudioSourceComponent::AudioSourceComponent(GameObject* ownerGameObject): Component(ownerGameObject,ComponentType::AUDIOSOURCE)
{
}

AudioSourceComponent::~AudioSourceComponent()
{
}

void AudioSourceComponent::SetEventInstance(FMOD::Studio::EventInstance* event)
{
	// Stop current audio
	if (mCurrentInstance != nullptr) {
		mCurrentInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
	}

	// Set to new event
	mCurrentInstance = event;

	// Play new audio
	if (mCurrentInstance != nullptr) {
		mCurrentInstance->start();
		mCurrentInstance->release();

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

void AudioSourceComponent::UpdateParameterValue(const char* name, float value)
{
	mCurrentInstance->setParameterByName(name, value);

	auto it = mParameters.find(name);
	if (it != mParameters.end()) {
		it->second = value;
	}

}

void AudioSourceComponent::Update()
{

}

Component* AudioSourceComponent::Clone(GameObject* owner) const
{
	return nullptr;
}

void AudioSourceComponent::Save(Archive& archive) const
{
}

void AudioSourceComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
}

void AudioSourceComponent::Enable()
{
}

void AudioSourceComponent::Disable()
{
}

void AudioSourceComponent::Reset()
{
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

		mParameters.insert(std::make_pair(parameter.name, parameter.defaultvalue));
	}

}
