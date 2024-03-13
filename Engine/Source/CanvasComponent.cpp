#include "CanvasComponent.h"
#include "float2.h"
#include "Application.h"
#include "ImageComponent.h"
#include "ModuleWindow.h"

CanvasComponent::CanvasComponent(bool active, GameObject* owner): Component(owner, ComponentType::CANVAS), 
	screenReferenceSize(1920, 1080),
	size(1920, 1080),
	screenFactor(0.0f) {

	//mImageComponent = new ImageComponent(reinterpret_cast<GameObject*>(this));

}

CanvasComponent::CanvasComponent(GameObject* owner) : Component(owner, ComponentType::CANVAS),
	screenReferenceSize(1920, 1080),
	size(1920, 1080),
	screenFactor(0.0f) {

	//mImageComponent = new ImageComponent(reinterpret_cast<GameObject*>(this));

}

CanvasComponent:: ~CanvasComponent() {
}

void CanvasComponent::Update() {};

Component* CanvasComponent::Clone(GameObject* owner) const {
	CanvasComponent* newComponent = new CanvasComponent(true, owner);
	return newComponent;
};
void CanvasComponent::Reset() {};

void CanvasComponent::RecalculateSizeAndScreenFactor()
{
	size = float2(App->GetWindow()->GetWidth(), App->GetWindow()->GetHeight());

	float2 factor = size.Div(screenReferenceSize);
	screenFactor = factor.x < factor.y ? factor.x : factor.y;
}

void CanvasComponent::Save(Archive& archive)const {};
void CanvasComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner) {};