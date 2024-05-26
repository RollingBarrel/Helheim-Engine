#include "Transform2DComponent.h"
#include "CanvasComponent.h"
#include "ImageComponent.h"
#include "ResourceTexture.h"
#include "Component.h"
#include "GameObject.h"

#include "Math/TransformOps.h"

Transform2DComponent::Transform2DComponent(GameObject* owner): Component(owner, ComponentType::TRANSFORM2D)
{
	CalculateMatrices();
}

Transform2DComponent::Transform2DComponent(const Transform2DComponent& original, GameObject* owner) : Component(owner, ComponentType::TRANSFORM2D)
{
	mPosition = original.mPosition;
	mEulerRotation = original.mEulerRotation;
	mRotation = original.mRotation;
	mSize = original.mSize;

	mAnchorMin = original.mAnchorMin;
	mAnchorMax = original.mAnchorMax;
	mPivot = original.mPivot;

	mLocalMatrix = original.mLocalMatrix;
	mGlobalMatrix = original.mGlobalMatrix;
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
	return new Transform2DComponent(*this, owner);
}

void Transform2DComponent::Reset()
{
	ResetTransform();
}

void Transform2DComponent::Save(JsonObject& obj) const
{
	Component::Save(obj);
	obj.AddFloats("Position", mPosition.ptr(), 3);
	obj.AddFloats("Rotation", mRotation.ptr(), 4);
	obj.AddFloats("Size", mSize.ptr(), 2);
	obj.AddFloats("AnchorMin", mAnchorMin.ptr(), 2);
	obj.AddFloats("AnchorMax", mAnchorMax.ptr(), 2);
	obj.AddFloats("Pivot", mPivot.ptr(), 2);
}

void Transform2DComponent::Load(const JsonObject& data)
{
	Component::Load(data);

	float pos[3];
	data.GetFloats("Position", pos);
	mPosition = float3(pos);

	float rot[4];
	data.GetFloats("Rotation", rot);
	mRotation = Quat(pos);

	float size[2];
	data.GetFloats("Size", size);
	mSize = float2(size);

	float anchorMin[2];
	data.GetFloats("AnchorMin", anchorMin);
	mAnchorMin = float2(anchorMin);

	float anchorMax[2];
	data.GetFloats("AnchorMax", anchorMax);
	mAnchorMax = float2(anchorMax);

	float pivot[2];
	data.GetFloats("Pivot", pivot);
	mPivot = float2(pivot);

	CalculateMatrices();
}

void Transform2DComponent::CalculateMatrices()
{
	mLocalMatrix = float4x4::FromTRS(mPosition, mRotation, float3(mSize, 1.0f));

	CanvasComponent* canvas = (CanvasComponent*)FindCanvasOnParents(this->GetOwner())->GetComponent(ComponentType::CANVAS);
	float2 canvasSize = canvas ? canvas->GetSize() : float2(1.0f, 1.0f); // Default to 1.0f if canvas size is not found

	GameObject* parent = GetOwner()->GetParent();
	if (parent)
	{
		Transform2DComponent* parentTransform = (Transform2DComponent*)parent->GetComponent(ComponentType::TRANSFORM2D);
		if (parentTransform)
		{
			// Get the parent global matrix
			float4x4 mParentMatrix = parentTransform->GetGlobalMatrix();
			mGlobalMatrix = float4x4::FromTRS(mParentMatrix.TranslatePart(), mParentMatrix.RotatePart(), float3::one.Div(mParentMatrix.ExtractScale()));

			// Translate the parent matrix by the transform translate
			float4x4 localMatrix = float4x4::FromTRS(mPosition, mRotation, float3(mSize, 1.0f));
			mGlobalMatrix = mGlobalMatrix.Mul(localMatrix);
		}
		else
		{
			mGlobalMatrix = mLocalMatrix;
		}
	}
	else
	{
		mGlobalMatrix = mLocalMatrix;
	}

	for (GameObject* child : GetOwner()->GetChildren())
	{
		Transform2DComponent* childTransform = (Transform2DComponent*) child->GetComponent(ComponentType::TRANSFORM2D);
		if (childTransform)
		{
			childTransform->CalculateMatrices();
		}
	}
}

void Transform2DComponent::RescaleMatrices(float2 ratio)
{
	mSize = float2(mSize.x * ratio.x, mSize.y * ratio.y);
	
	for (GameObject* child : GetOwner()->GetChildren())
	{
		Transform2DComponent* childTransform = (Transform2DComponent*)child->GetComponent(ComponentType::TRANSFORM2D);
		if (childTransform)
		{
			childTransform->RescaleMatrices(ratio);
		}
	}
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
	mPosition = position; 
	CalculateMatrices();
}

void Transform2DComponent::SetRotation(const float3& rotation)
{ 
	mRotation = Quat::FromEulerXYZ(rotation.x, rotation.y, rotation.z); 
	mEulerRotation = rotation; 
	CalculateMatrices(); 
}

void Transform2DComponent::SetSize(const float2 size)
{ 
	RescaleMatrices(size.Div(mSize));
	CalculateMatrices(); 
}

void Transform2DComponent::SetAnchorMin(const float2 anchorMin)
{
	mAnchorMin = anchorMin;
	CalculateMatrices();
}

void Transform2DComponent::SetAnchorMax(const float2 anchorMax)
{
	mAnchorMax = anchorMax;
	CalculateMatrices();
}

void Transform2DComponent::SetPivot(const float2 pivot)
{
	mPivot = pivot;
	CalculateMatrices();
}

GameObject* Transform2DComponent::FindCanvasOnParents(GameObject* gameObject)
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
