#include "Transform2DComponent.h"
#include "CanvasComponent.h"
#include "Component.h"
#include "GameObject.h"

#include "Math/TransformOps.h"

Transform2DComponent::Transform2DComponent(const bool active, GameObject* owner): Component(owner, ComponentType::TRANSFORM2D)
{
}

Transform2DComponent::~Transform2DComponent()
{
}

void Transform2DComponent::Update() 
{
}

void Transform2DComponent::CalculateMatrices()
{
	localMatrix = float4x4::FromTRS(GetPositionRelativeToParent(), rotation, scale) *
		float4x4::Translate(float3((-pivot + float2(0.5f, 0.5f)).Mul(size), 0.0f));

	const GameObject* parent = GetOwner()->GetParent();

	if (parent)
	{
		Transform2DComponent* parentTransform = (Transform2DComponent*) parent->GetComponent(ComponentType::TRANSFORM2D);
		if (parentTransform)
		{
			globalMatrix = parentTransform->GetGlobalMatrix().Mul(localMatrix);
		}
		else
		{
			globalMatrix = localMatrix;
		}
	}

	for (GameObject* child : GetOwner()->GetChildren())
	{
		Transform2DComponent* childTransform = (Transform2DComponent*) child->GetComponent(ComponentType::TRANSFORM2D);
		childTransform->CalculateMatrices();
	}
}

float3 Transform2DComponent::GetPositionRelativeToParent()
{
	float2 parentSize(0, 0);

	GameObject* parent = GetOwner()->GetParent();
	if (parent != nullptr)
	{
		CanvasComponent* parentCanvas = (CanvasComponent*) parent->GetComponent(ComponentType::CANVAS);
		Transform2DComponent* parentTransform2D = (Transform2DComponent*) parent->GetComponent(ComponentType::TRANSFORM2D);

		if (parentTransform2D != nullptr)
		{
			if (parentCanvas != nullptr)
			{
				parentSize = parentCanvas->GetSize() / parentCanvas->GetScreenFactor();
			}
			else
			{
				parentSize = parentTransform2D->GetSize();
			}
		}
	}

	float3 positionRelativeToParent;
	positionRelativeToParent.x = position.x + (parentSize.x * (anchorMin.x - 0.5f));
	positionRelativeToParent.y = position.y + (parentSize.y * (anchorMin.y - 0.5f));
	positionRelativeToParent.z = position.z;
	return positionRelativeToParent;
}

float3 Transform2DComponent::GetScreenPosition()
{
	float3 screenPosition = GetPositionRelativeToParent();
	GameObject* parent = GetOwner()->GetParent();

	while (parent != nullptr)
	{
		Transform2DComponent* parentTransform2D = (Transform2DComponent*)parent->GetComponent(ComponentType::TRANSFORM2D);
		if (parentTransform2D == nullptr)
			break;
		screenPosition += parentTransform2D->GetPositionRelativeToParent();
		parent = parent->GetParent();
	}

	return screenPosition;
}