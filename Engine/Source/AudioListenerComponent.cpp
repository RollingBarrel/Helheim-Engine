#include "AudioListenerComponent.h"

#include "ModuleAudio.h"
#include "GameObject.h"
#include "Application.h"
#include "fmod_studio.hpp"
AudioListenerComponent::AudioListenerComponent(GameObject* ownerGameObject) : Component(ownerGameObject, ComponentType::AUDIOLISTENER)
{
	
}

AudioListenerComponent::AudioListenerComponent(const AudioListenerComponent& original, GameObject* ownerGameObject) : Component(ownerGameObject, ComponentType::AUDIOLISTENER)
{
}

AudioListenerComponent::~AudioListenerComponent()
{
		
}

void AudioListenerComponent::Update()
{
	float3 gameobjectPosition = GetOwner()->GetPosition();

	FMOD::Studio::System* system = App->GetAudio()->GetFMODSystem();
	FMOD_3D_ATTRIBUTES attributes = { { 0 } };

	attributes.position.x = gameobjectPosition.x;
	attributes.position.z = gameobjectPosition.z;
	attributes.forward.z = 1.0f;
	attributes.up.y = 1.0f;
	system->setListenerAttributes(0, &attributes);
}

Component* AudioListenerComponent::Clone(GameObject* owner) const
{
	return new AudioListenerComponent(*this, owner);
}

void AudioListenerComponent::Save(JsonObject& obj) const
{
	Component::Save(obj);
}

void AudioListenerComponent::Load(const JsonObject& data, GameObject* owner)
{
	Component::Load(data, owner);
}

void AudioListenerComponent::Enable()
{
}

void AudioListenerComponent::Disable()
{
	// TODO (to be implement when is needed - maybe use masterChannelGroup.setMute(true);)
}

void AudioListenerComponent::Reset()
{
}
