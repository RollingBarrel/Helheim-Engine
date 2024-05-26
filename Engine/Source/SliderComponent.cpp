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
	mFillPercent = original.mFillPercent;
	mCanvas = original.mCanvas;
}

SliderComponent::SliderComponent(GameObject* owner) : Component(owner, ComponentType::SLIDER)
{
	mCanvas = (CanvasComponent*)(FindCanvasOnParents(this->GetOwner())->GetComponent(ComponentType::CANVAS));
	GameObject* self = (GameObject*)this->GetOwner();

	if (self->GetChildren().empty()) 
	{
		self->SetName("Slider");
		mSliderTransform2D = new Transform2DComponent(self);

		mBackground = this->GetOwner()->GetChildren()[0];
		mFill = this->GetOwner()->GetChildren()[1];

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
	
	// Set default values
	//mBgImage->SetColor(float3(1.f, 0, 0));
	//mFillImage->SetColor(float3(0, 1.f, 0));
	//mSliderTransform2D->SetSize(float2(500, 50));
	//mBgTransform2D->SetSize(float2::one);

	//mFillTransform2D->SetPosition(float3(((1 - mFillPercent) / 2) * -1, 0, 0));
	//mFillTransform2D->SetSize(float2(mFillPercent, 1));
}

void SliderComponent::SetFillPercent(float fillPercent)
{
	if (mFillTransform2D == nullptr) mFillTransform2D = (Transform2DComponent*)mFill->GetComponent(ComponentType::TRANSFORM2D);
	if (mBackground == nullptr) mBgTransform2D = (Transform2DComponent*)mBackground->GetComponent(ComponentType::TRANSFORM2D);

	this->mFillPercent = fillPercent;

	if (!mCanvas) return;
	if (mCanvas->GetScreenSpace()) 
	{
		mFillTransform2D->SetPosition(float3(((1 - fillPercent) / 2) * -1, 0, 0));
		mFillTransform2D->SetSize(float2(fillPercent, mFillTransform2D->GetSize().y));
	}
	else 
	{
		mFill->SetPosition(float3(((1 - fillPercent) / 2) * -1, 0, 0));
		mFill->SetScale(float3(fillPercent, mFillTransform2D->GetSize().y, 0));
	}
	
}

void SliderComponent::Save(JsonObject& obj) const
{
	Component::Save(obj);
}

void SliderComponent::Load(const JsonObject& data)
{
	Component::Load(data);
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

