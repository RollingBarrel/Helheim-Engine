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

void CanvasComponent::Save(JsonObject& obj)const 
{
	Component::Save(obj);
	
	obj.AddFloats("Size", mSize.ptr(), 2);
}

void CanvasComponent::Load(const JsonObject& data)
{
	Component::Load(data);
	
	float size[2];
	data.GetFloats("Size", size);
	mSize = float2(size);
}
