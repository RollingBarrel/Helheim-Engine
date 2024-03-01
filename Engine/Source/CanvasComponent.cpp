#include "CanvasComponent.h"
#include "float2.h"

#include "ImageComponent.h"

CanvasComponent::CanvasComponent(bool active, GameObject* owner): Component(owner, ComponentType::CANVAS), 
	screenReferenceSize(1920, 1080),
	size(1920, 1080),
	screenFactor(0.0f) {

	mImageComponent = new ImageComponent(reinterpret_cast<GameObject*>(this));

	}


CanvasComponent:: ~CanvasComponent() {
}

void CanvasComponent::Update() {};

void CanvasComponent::Draw() {
mImageComponent->Draw();
}

Component* CanvasComponent::Clone(GameObject* owner) const {
	CanvasComponent* newComponent = new CanvasComponent(true, owner);
	return newComponent;
};
void CanvasComponent::Reset() {};

void CanvasComponent::Save(Archive& archive)const {};
void CanvasComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner) {};