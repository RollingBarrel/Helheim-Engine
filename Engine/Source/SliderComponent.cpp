#pragma once
#include "SliderComponent.h"
#include "ImageComponent.h"
#include "CanvasComponent.h"

SliderComponent::SliderComponent(GameObject* owner, bool active) : Component(owner, ComponentType::SLIDER)
{
	mCanvas = (CanvasComponent*)(FindCanvasOnParents(this->GetOwner())->GetComponent(ComponentType::CANVAS));
}

SliderComponent::SliderComponent(const SliderComponent& original, GameObject* owner) : Component(owner, ComponentType::SLIDER)
{
	mValue = original.mValue;
	mCanvas = original.mCanvas;
}

SliderComponent::SliderComponent(GameObject* owner) : Component(owner, ComponentType::SLIDER)
{
	mCanvas = (CanvasComponent*)(FindCanvasOnParents(this->GetOwner())->GetComponent(ComponentType::CANVAS));
	GameObject* self = (GameObject*)this->GetOwner();

	if (self->GetChildren().empty()) 
	{
		self->SetName("Slider");
		mFill = new GameObject("Fill", self);
		mBackground = new GameObject("Background", self);

		mSliderTransform2D = new Transform2DComponent(self);
		mBgImage = new ImageComponent(mBackground);
		mFillImage = new ImageComponent(mFill);
		mBgTransform2D = new Transform2DComponent(mBackground);
		mFillTransform2D = new Transform2DComponent(mFill);

		self->AddComponent(mSliderTransform2D, this);
		mBackground->AddComponent(mBgTransform2D, this);
		mBackground->AddComponent(mBgImage, this);
		mFill->AddComponent(mFillTransform2D, this);
		mFill->AddComponent(mFillImage, this);
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

void SliderComponent::SetValue(float fillPercent)
{
	if (mFillTransform2D == nullptr) mFillTransform2D = (Transform2DComponent*)mFill->GetComponent(ComponentType::TRANSFORM2D);
	if (mBgTransform2D == nullptr) mBgTransform2D = (Transform2DComponent*)mBackground->GetComponent(ComponentType::TRANSFORM2D);

	this->mValue = fillPercent;

	if (!mCanvas) return;

	float backgroundWidth = mBgTransform2D->GetSize().x;
	float fillWidth = backgroundWidth * fillPercent;
	float fillPositionX = (backgroundWidth - fillWidth) / 2 * -1; // Center the fill

	if (mCanvas->GetScreenSpace())
	{
		mFillTransform2D->SetPosition(float3(fillPositionX, 0, 0));
		mFillTransform2D->SetSize(float2(fillWidth, mFillTransform2D->GetSize().y));
	}
	else
	{
		mFill->SetPosition(float3(fillPositionX, 0, 0));
		mFill->SetScale(float3(fillPercent, 1.0f, 1.0f));
	}
}

void SliderComponent::Save(Archive& archive) const
{
	Component::Save(archive);
}

void SliderComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
	Component::LoadFromJSON(data, owner);
}

SliderComponent:: ~SliderComponent()
{
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

