#include "AudioListenerComponent.h"

AudioListenerComponent::AudioListenerComponent(GameObject* ownerGameObject) : Component(ownerGameObject, ComponentType::AUDIOSOURCE)
{
}

AudioListenerComponent::~AudioListenerComponent()
{
}
