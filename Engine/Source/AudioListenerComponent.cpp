#include "AudioListenerComponent.h"

#include "ModuleAudio.h"
#include "GameObject.h"
#include "Application.h"
#include "FmodUtils.h"

AudioListenerComponent::AudioListenerComponent(GameObject* ownerGameObject) : Component(ownerGameObject, ComponentType::AUDIOLISTENER)
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
	return nullptr;
}

void AudioListenerComponent::Save(Archive& archive) const
{
}

void AudioListenerComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
}

void AudioListenerComponent::Enable()
{
}

void AudioListenerComponent::Disable()
{
}

void AudioListenerComponent::Reset()
{
}
