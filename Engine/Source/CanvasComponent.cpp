#include "CanvasComponent.h"
#include "float2.h"

CanvasComponent::CanvasComponent(bool active, GameObject* owner): Component(owner, ComponentType::CANVAS), 
	screenReferenceSize(1920, 1080),
	size(1920, 1080),
	screenFactor(0.0f) {
	
}

CanvasComponent:: ~CanvasComponent() {
}