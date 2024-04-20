#pragma once
#include "SliderComponent.h"

SliderComponent::SliderComponent(GameObject* owner, bool active) : Component(owner, ComponentType::SLIDER)
{
}

SliderComponent::SliderComponent(GameObject* owner) : Component(owner, ComponentType::SLIDER)
{
	GameObject* self = (GameObject*)this->GetOwner();

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

SliderComponent:: ~SliderComponent()
{
}

Component* SliderComponent::Clone(GameObject* origin) const
{
	return nullptr;
}

