#include "LightSourceComponent.h"

LightSourceComponent::LightSourceComponent(GameObject* owner = nullptr, const PointLight& light) : Component(owner, ComponentType::LIGHTSOURCE), mData(&light) {}

LightSourceComponent::~LightSourceComponent() {}

