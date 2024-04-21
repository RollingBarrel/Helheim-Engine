#pragma once
#include "SliderComponent.h"

SliderComponent::SliderComponent(GameObject* owner, bool active) : Component(owner, ComponentType::SLIDER)
{
}

SliderComponent::SliderComponent(const SliderComponent& original, GameObject* owner) : Component(owner, ComponentType::SLIDER)
{
	mFillPercent = original.mFillPercent;
	mFill = new GameObject(original.mFill);
	mBackground = new GameObject(original.mBackground);
	mSliderTransform2D = new Transform2DComponent(*original.mSliderTransform2D, GetOwner());
	mBgImage = new ImageComponent(*original.mBgImage, GetOwner());
	mFillImage = new ImageComponent(*original.mFillImage, GetOwner());;
	mBgTransform2D = new Transform2DComponent(*original.mBgTransform2D, GetOwner());
	mFillTransform2D = new Transform2DComponent(*original.mFillTransform2D, GetOwner());
}

SliderComponent::SliderComponent(GameObject* owner) : Component(owner, ComponentType::SLIDER)
{
	GameObject* self = (GameObject*)this->GetOwner();

	if (self->GetChildren().empty()) {
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
	
	// Set default values
	mBgImage->SetColor(float3(1.f, 0, 0));
	mFillImage->SetColor(float3(0, 1.f, 0));
	mSliderTransform2D->SetSize(float2(500, 50));
	mBgTransform2D->SetSize(float2::one);

	mFillTransform2D->SetPosition(float3(((1 - mFillPercent) / 2) * -1, 0, 0));
	mFillTransform2D->SetSize(float2(mFillPercent, 1));
}

void SliderComponent::SetFillPercent(float fillPercent)
{
	this->mFillPercent = fillPercent;
	mFillTransform2D->SetPosition(float3(((1 - fillPercent) / 2) * -1, 0, 0));
	mFillTransform2D->SetSize(float2(fillPercent, 1));
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

