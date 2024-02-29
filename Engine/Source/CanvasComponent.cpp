#include "CanvasComponent.h"

CanvasComponent::CanvasComponent(bool active, GameObject* owner): Component(owner, ComponentType::CANVAS) {
}

CanvasComponent:: ~CanvasComponent() {
}