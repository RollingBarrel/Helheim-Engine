#include "CanvasComponent.h"
#include "float2.h"
#include "Application.h"
#include "ImageComponent.h"
#include "ModuleWindow.h"
#include "Application.h"
#include "ScenePanel.h"
#include "ModuleEditor.h"
#include "ModuleUI.h"

CanvasComponent::CanvasComponent(GameObject* owner) : Component(owner, ComponentType::CANVAS),
	mSize(1920, 1080) 
{
	App->GetUI()->AddCanvas(this->GetOwner());
}

CanvasComponent::CanvasComponent(const CanvasComponent& original, GameObject* owner) :Component(owner, ComponentType::CANVAS)
{
	App->GetUI()->AddCanvas(this->GetOwner());
	mSize = original.mSize;
}

CanvasComponent:: ~CanvasComponent() 
{
	App->GetUI()->RemoveCanvas(this->GetOwner());
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
