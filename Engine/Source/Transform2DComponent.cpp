#include "Transform2DComponent.h"
#include "CanvasComponent.h"
#include "Component.h"
#include "GameObject.h"

#include "Math/TransformOps.h"

Transform2DComponent::Transform2DComponent(GameObject* owner): Component(owner, ComponentType::TRANSFORM2D)
{
	CalculateMatrices();
}

Transform2DComponent::Transform2DComponent(const bool active, GameObject* owner) : Component(owner, ComponentType::TRANSFORM2D)
{
	CalculateMatrices();
}

Transform2DComponent::~Transform2DComponent()
{
}

void Transform2DComponent::Update() 
{
}

Component* Transform2DComponent::Clone(GameObject* owner) const
{
	return nullptr;
}

void Transform2DComponent::Reset()
{
}

void Transform2DComponent::Save(Archive& archive) const
{
}

void Transform2DComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
}

void Transform2DComponent::CalculateMatrices()
{
	mLocalMatrix = float4x4::FromTRS(GetPositionRelativeToParent(), mRotation, float3(mSize,0))
		/** float4x4::Translate(float3((-mPivot + float2(0.5f, 0.5f))/*.Mul(mSize), 0.0f))*/;

	GameObject* parent = GetOwner()->GetParent();

	if (parent)
	{
		Transform2DComponent* parentTransform = (Transform2DComponent*)parent->GetComponent(ComponentType::TRANSFORM2D);
		if (parentTransform)
		{
			mGlobalMatrix = parentTransform->GetGlobalMatrix().Mul(mLocalMatrix);
		}
		else
		{
			mGlobalMatrix = mLocalMatrix;
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
	positionRelativeToParent.x = mPosition.x + (parentSize.x * (mAnchorMin.x - 0.5f));
	positionRelativeToParent.y = mPosition.y + (parentSize.y * (mAnchorMin.y - 0.5f));
	positionRelativeToParent.z = mPosition.z;
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

void Transform2DComponent::ResetTransform() {
	mPosition = float3::zero;
	mEulerRotation = float3::zero;
	mRotation = Quat::identity;
	mSize = float2::one;

	mAnchorMin = float2(0.5, 0.5);
	mAnchorMax = float2(0.5, 0.5);
	mPivot = float2::zero;

	mLocalMatrix = float4x4::identity;
	mGlobalMatrix = float4x4::identity;

	CalculateMatrices();
}

void Transform2DComponent::SetPosition(const float3& position)
{ 
	mPosition = position; CalculateMatrices();
}

void Transform2DComponent::SetRotation(const float3& rotation)
{ 
	mRotation = Quat::FromEulerXYZ(rotation.x, rotation.y, rotation.z); 
	mEulerRotation = rotation; 
	CalculateMatrices(); 
}

void Transform2DComponent::SetSize(const float2 size)
{ 
	mSize = size; 
	CalculateMatrices(); 
}