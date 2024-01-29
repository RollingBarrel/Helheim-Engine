#include "LightSourceComponent.h"

LightSourceComponent::LightSourceComponent(GameObject* owner = nullptr) : Component(owner, ComponentType::LIGHTSOURCE) {}

LightSourceComponent::~LightSourceComponent() {}