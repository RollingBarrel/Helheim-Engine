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
	// For events
	float3 gameobjectPosition = GetOwner()->GetWorldPosition();

	FMOD::Studio::System* system = App->GetAudio()->GetFMODSystem();
	FMOD_3D_ATTRIBUTES attributes = { { 0 } };

	attributes.position.x = gameobjectPosition.x;
	attributes.position.z = gameobjectPosition.z;
	attributes.forward.z = 1.0f;
	attributes.up.y = 1.0f;
	system->setListenerAttributes(0, &attributes);

	// For channels
	FMOD::System* core = App->GetAudio()->GetFMODCoreSystem();
	core->set3DListenerAttributes(0, &attributes.position, &attributes.velocity, &attributes.forward, &attributes.up);
}

Component* AudioListenerComponent::Clone(GameObject* owner) const
{
	return new AudioListenerComponent(*this, owner);
}

void AudioListenerComponent::Save(JsonObject& obj) const
{
	Component::Save(obj);
}

void AudioListenerComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
	Component::Load(data, uidPointerMap);
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
