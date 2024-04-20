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

	//GameObject* background = new GameObject("Background", self);
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
	//sliderTransform2D->SetPosition(float3(750, -500, 0));
	sliderTransform2D->SetSize(float2(400, 50));

	float3 bgPosition = sliderTransform2D->GetPosition();

	fillTransform2D->SetPosition()
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





