#pragma once
#include "SliderComponent.h"

#include "ImageComponent.h"
#include "CanvasComponent.h"
#include "GameObject.h"
#include "Transform2DComponent.h"

SliderComponent::SliderComponent(GameObject* owner) : Component(owner, ComponentType::SLIDER)
{
	mCanvas = (CanvasComponent*)(FindCanvasOnParents(this->GetOwner())->GetComponent(ComponentType::CANVAS));
	GameObject* self = (GameObject*)this->GetOwner();

	if (self->GetChildren().empty()) 
	{
		self->SetName("Slider");
		mSliderTransform2D = new Transform2DComponent(self);

		mBackground = new GameObject("Background", self);
		mFill = new GameObject("Fill", self);

		self->CreateComponent(ComponentType::TRANSFORM2D);
		mBackground->CreateComponent(ComponentType::TRANSFORM2D);
		mBackground->CreateComponent(ComponentType::IMAGE);
		mFill->CreateComponent(ComponentType::TRANSFORM2D);
		mFill->CreateComponent(ComponentType::IMAGE);
	}
	else 
	{
		const std::vector<GameObject*>& childrens = GetOwner()->GetChildren();
		for (GameObject* child : childrens)
		{
			if (child->GetName() == "Fill") 
			{
				mFill = child;
			}
			else if (child->GetName() == "Background")
			{
				mBackground = child;
			}
		}
	}
}

SliderComponent::SliderComponent(const SliderComponent& original, GameObject* owner) : Component(owner, ComponentType::SLIDER)
{
	mValue = original.mValue;
	mCanvas = original.mCanvas;
}

SliderComponent:: ~SliderComponent()
{
}

void SliderComponent::SetValue(float fillPercent)
{
	if (mFillTransform2D == nullptr) mFillTransform2D = (Transform2DComponent*)mFill->GetComponent(ComponentType::TRANSFORM2D);
	if (mBgTransform2D == nullptr) mBgTransform2D = (Transform2DComponent*)mBackground->GetComponent(ComponentType::TRANSFORM2D);

	this->mValue = fillPercent;

	if (!mCanvas) return;

	float backgroundWidth = mBgTransform2D->GetSize().x;
	float fillWidth = backgroundWidth * fillPercent;
	float fillPositionX = (backgroundWidth - fillWidth) / 2 * -1; // Center the fill

	if (mCanvas->GetRenderSpace() == RenderSpace::Screen)
	{
		mFillTransform2D->SetPosition(float3(fillPositionX, 0, 0));
		mFillTransform2D->SetSize(float2(fillWidth, mFillTransform2D->GetSize().y));
	}
	else
	{
		mFill->SetWorldPosition(float3(fillPositionX, 0, 0));
		mFill->SetWorldScale(float3(fillPercent, 1.0f, 1.0f));
	}
}

void SliderComponent::Save(JsonObject& obj) const
{
	Component::Save(obj);
}

void SliderComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
	Component::Load(data, uidPointerMap);
}

Component* SliderComponent::Clone(GameObject* origin) const
{
	return new SliderComponent(*this, origin);
}

GameObject* SliderComponent::FindCanvasOnParents(GameObject* gameObject)
{
	if (gameObject == nullptr)
	{
		return nullptr;
	}

	GameObject* currentObject = gameObject;

	while (currentObject != nullptr)
	{
		if (currentObject->GetComponent(ComponentType::CANVAS) != nullptr)
		{
			return currentObject;
		}
		currentObject = currentObject->GetParent();
	}

	return nullptr; // No canvas found on parents
}

