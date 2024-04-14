#include "AudioSourceComponent.h"
#include "FmodUtils.h"

#include "GameObject.h"
#include "Application.h"

AudioSourceComponent::AudioSourceComponent(GameObject* ownerGameObject): Component(ownerGameObject,ComponentType::AUDIOSOURCE)
{
}

AudioSourceComponent::~AudioSourceComponent()
{
}

void AudioSourceComponent::SetEventInstance(FMOD::Studio::EventInstance* event)
{
	// Stop current audio
	if (mEventInstance != nullptr) {
		//mEventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
	}

	// Set to new event
	mEventInstance = event;

	// Play new audio
	if (mEventInstance != nullptr) {
		//mEventInstance->start();
		//mEventInstance->release();

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
	mEventInstance->setParameterByName(name, value);

	auto it = mParameters.find(name);
	if (it != mParameters.end()) {
		it->second = value;
	}

}

void AudioSourceComponent::Update()
{
	//if (triggerEvent) {
	//	// UPDATE 3D parameters
	//	float3 gameobjectPosition = GetOwner()->GetPosition();

	//	FMOD_3D_ATTRIBUTES attributes = { { 0 } };

	//	attributes.position.x = gameobjectPosition.x;
	//	attributes.position.z = gameobjectPosition.z;

	//	attributes.forward.z = 1.0f;
	//	attributes.up.y = 1.0f;

	//	// Play Audio
	//	mEventInstance->set3DAttributes(&attributes);
	//	mEventInstance->start();
	//	mEventInstance->release();
	//}
	//else {
	//	mEventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
	//}
}

void AudioSourceComponent::OnUpdate(bool triggerEvent)
{
	
	if (triggerEvent) {
		// UPDATE 3D parameters
		float3 gameobjectPosition = GetOwner()->GetPosition();

		FMOD_3D_ATTRIBUTES attributes = { { 0 } };

		attributes.position.x = gameobjectPosition.x;
		attributes.position.z = gameobjectPosition.z;

		attributes.forward.z = 1.0f;
		attributes.up.y = 1.0f;

		// Play Audio
		mEventInstance->set3DAttributes(&attributes);
		mEventInstance->start();
		mEventInstance->release();
	}
	else {
		mEventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
	}


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
