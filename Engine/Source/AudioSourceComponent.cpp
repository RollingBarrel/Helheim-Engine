#include "AudioSourceComponent.h"

AudioSourceComponent::AudioSourceComponent(GameObject* ownerGameObject): Component(ownerGameObject,ComponentType::AUDIOSOURCE)
{
}

AudioSourceComponent::~AudioSourceComponent()
{
}
