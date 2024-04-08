#include "CanvasComponent.h"
#include "float2.h"
#include "Application.h"
#include "ImageComponent.h"
#include "ModuleWindow.h"

CanvasComponent::CanvasComponent(bool active, GameObject* owner): Component(owner, ComponentType::CANVAS), 
	mScreenReferenceSize(1920, 1080),
	mSize(1920, 1080),
	mScreenFactor(0.0f) {
}

CanvasComponent::CanvasComponent(GameObject* owner) : Component(owner, ComponentType::CANVAS),
	mScreenReferenceSize(1920, 1080),
	mSize(1920, 1080),
	mScreenFactor(0.0f) {
}

CanvasComponent:: ~CanvasComponent() 
{
}

void CanvasComponent::Update() 
{

};

Component* CanvasComponent::Clone(GameObject* owner) const 
{
	CanvasComponent* newComponent = new CanvasComponent(true, owner);
	return newComponent;
};

void CanvasComponent::Reset() 
{

};

void CanvasComponent::RecalculateSizeAndScreenFactor()
{
	mSize = float2(App->GetWindow()->GetWidth(), App->GetWindow()->GetHeight());

	float2 factor = mSize.Div(mScreenReferenceSize);
	mScreenFactor = factor.x < factor.y ? factor.x : factor.y;
}

void CanvasComponent::Save(Archive& archive)const 
{
	Component::Save(archive);

	archive.AddFloat2("Size", mSize);
	archive.AddFloat("ScreenFactor", mScreenFactor);
	archive.AddFloat2("ScreenReferenceSize", mScreenReferenceSize);
};

void CanvasComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner) 
{
	Component::LoadFromJSON(data, owner);

	if (data.HasMember("Size") && data["Size"].IsArray()) {
		const rapidjson::Value& values = data["Size"];
		float x{ 0.0f }, y{ 0.0f };
		if (values.Size() == 2 && values[0].IsFloat() && values[1].IsFloat()) {
			x = values[0].GetFloat();
			y = values[1].GetFloat();
		}

		mSize = float2(x, y);
	}
	if (data.HasMember("ScreenFactor") && data["ScreenFactor"].IsFloat()) {
		mScreenFactor = data["ScreenFactor"].GetFloat();
	}
	if (data.HasMember("ScreenReferenceSize") && data["ScreenReferenceSize"].IsArray()) {
		const rapidjson::Value& values = data["ScreenReferenceSize"];
		float x{ 0.0f }, y{ 0.0f };
		if (values.Size() == 2 && values[0].IsFloat() && values[1].IsFloat()) {
			x = values[0].GetFloat();
			y = values[1].GetFloat();
		}

		mScreenReferenceSize = float2(x, y);
	}

	RecalculateSizeAndScreenFactor();
};
