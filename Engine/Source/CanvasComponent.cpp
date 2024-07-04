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
	obj.AddInt("Render", static_cast<int>(mRenderSpace));
}

void CanvasComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
	Component::Load(data, uidPointerMap);
	
	float size[2];
	if (data.HasMember("Size")) data.GetFloats("Size", size);

	// This is here for backwards compatibility after swaping ScreenSpace for the Render variable
	// It can be deleted in future updates
	if (data.HasMember("ScreenSpace"))
	{
		if (data.GetBool("ScreenSpace"))
		{
			mRenderSpace = RenderSpace::Screen;
		}
		else mRenderSpace = RenderSpace::World;
	}

	mSize = float2(size);
	if (data.HasMember("Render"))
	{
		mRenderSpace = static_cast<RenderSpace>(data.GetInt("Render"));
	}
	
}
