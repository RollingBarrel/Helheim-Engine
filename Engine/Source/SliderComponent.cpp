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
	GameObject* background = new GameObject("Background", self);

	Transform2DComponent* sliderTransform2D = new Transform2DComponent(self);
	ImageComponent* bgImage = new ImageComponent(background);
	ImageComponent* fillImage = new ImageComponent(fill);
	Transform2DComponent* bgTransform2D = new Transform2DComponent(background);
	Transform2DComponent* fillTransform2D = new Transform2DComponent(fill);
	
	self->AddComponent(sliderTransform2D, this);
	background->AddComponent(bgTransform2D, this);
	background->AddComponent(bgImage, this);
	fill->AddComponent(fillTransform2D, this);
	fill->AddComponent(fillImage, this);

	// Set default values
	bgImage->SetColor(float3(1.f, 0, 0));
	fillImage->SetColor(float3(0, 1.f, 0));
	sliderTransform2D->SetSize(float2(400, 50));
	bgTransform2D->SetSize(float2::one);

	const float3 fillPosition = float3((1-fillPercent)/2, 0, 0);

	fillTransform2D->SetPosition(float3(((1 - fillPercent) / 2) * -1, 0, 0));
	fillTransform2D->SetSize(float2(fillPercent, 1));
}

void SliderComponent::SetFillPercent(float fillPercent)
{
	this->fillPercent = fillPercent;
}



SliderComponent:: ~SliderComponent()
{
}

Component* SliderComponent::Clone(GameObject* origin) const
{
	return nullptr;
}




