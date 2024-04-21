#include "CanvasComponent.h"
#include "float2.h"
#include "Application.h"
#include "ImageComponent.h"
#include "ModuleWindow.h"
#include "Application.h"
#include "ScenePanel.h"
#include "ModuleEditor.h"

CanvasComponent::CanvasComponent(bool active, GameObject* owner): Component(owner, ComponentType::CANVAS), 
	mSize(1920, 1080) 
{
}

CanvasComponent::CanvasComponent(GameObject* owner) : Component(owner, ComponentType::CANVAS),
	mSize(1920, 1080) 
{
}

CanvasComponent::CanvasComponent(const CanvasComponent& original, GameObject* owner) :Component(owner, ComponentType::CANVAS)
{
	mSize = original.mSize;
}

CanvasComponent:: ~CanvasComponent() 
{
}

void CanvasComponent::Update() 
{
}

Component* CanvasComponent::Clone(GameObject* owner) const 
{
	return new CanvasComponent(*this, owner);
}

void CanvasComponent::Reset() 
{
}

void CanvasComponent::Save(Archive& archive)const 
{
	Component::Save(archive);

	archive.AddFloat2("Size", mSize);
}

void CanvasComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner) 
{
	Component::LoadFromJSON(data, owner);

	if (data.HasMember("Size") && data["Size"].IsArray()) 
	{
		const rapidjson::Value& values = data["Size"];
		float x{ 0.0f }, y{ 0.0f };
		if (values.Size() == 2 && values[0].IsFloat() && values[1].IsFloat()) 
		{
			x = values[0].GetFloat();
			y = values[1].GetFloat();
		}

		mSize = float2(x, y);
	}
}
