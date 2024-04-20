#include "SliderComponent.h"
#include "ImageComponent.h"
#include "GameObject.h"
#include "ButtonComponent.h"
#include "Transform2DComponent.h"
#include "Math/float3.h"

class GameObject;

SliderComponent::SliderComponent(GameObject* owner, bool active) : Component(owner, ComponentType::SLIDER)
{
}

SliderComponent::SliderComponent(GameObject* owner) : Component(owner, ComponentType::SLIDER)
{
	Init();
}

void SliderComponent::Init()
{
	GameObject* self = (GameObject*)this->GetOwner();
	self->SetName("Slider");

	GameObject* fill = new GameObject("Fill", self);

	Transform2DComponent* sliderTransform2D = new Transform2DComponent(this->GetOwner());
	ImageComponent* bgImage = new ImageComponent(this->GetOwner());
	ImageComponent* fillImage = new ImageComponent(fill);
	Transform2DComponent* fillTransform2D = new Transform2DComponent(fill);
	
	self->AddComponent(sliderTransform2D, this);
	self->AddComponent(bgImage, this);
	fill->AddComponent(fillTransform2D, this);
	fill->AddComponent(fillImage, this);

	// Set default values
	bgImage->SetColor(float3(1.f, 0, 0));
	fillImage->SetColor(float3(0, 1.f, 0));
	sliderTransform2D->SetSize(float2(400, 50));

	const float3 bgPosition = sliderTransform2D->GetPosition();
	const float3 fillPosition = float3(bgPosition.x-(fillTransform2D->GetSize().x)/2, bgPosition.y, bgPosition.z);

	fillTransform2D->SetPosition(fillPosition);
}



SliderComponent:: ~SliderComponent()
{
}

Component* SliderComponent::Clone(GameObject* origin) const
{
	return nullptr;
}

void SliderComponent::Draw()
{
//	mSliderFill->Draw();
//mBackground->Draw();

}





