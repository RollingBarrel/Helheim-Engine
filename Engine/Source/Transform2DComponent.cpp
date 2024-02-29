#include "Transform2DComponent.h"

Transform2DComponent::Transform2DComponent(const bool active, GameObject* owner): Component(owner, ComponentType::TRANSFORM2D)
{
}

Transform2DComponent::~Transform2DComponent()
{
}

void Transform2DComponent::Update() {
}

