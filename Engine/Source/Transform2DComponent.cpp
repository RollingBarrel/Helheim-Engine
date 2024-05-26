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
		if (childTransform)
		{
			childTransform->CalculateMatrices();
		}
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

		if (parentCanvas != nullptr)
		{
			parentSize = parentCanvas->GetSize(); // / parentCanvas->GetScreenFactor();
		}
		else
		{
			if (parentTransform2D != nullptr)
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
	/*ImageComponent* component = ((ImageComponent*)GetOwner()->GetComponent(ComponentType::IMAGE));
	if ( component->GetMantainRatio() )
	{
		if (size.x != mSize.x)
		{
			float originalRatio = component->GetImage()->GetWidth() / component->GetImage()->GetHeight();
			float currentRatio = size.x / size.y;
			float ratio = currentRatio / originalRatio;
			float2 newSize = float2(mSize.x * ratio, mSize.y);
			mSize = newSize;
		}
		else 
		{
			float originalRatio = component->GetImage()->GetWidth() / component->GetImage()->GetHeight();
			float currentRatio = size.x / size.y;
			float ratio = currentRatio / originalRatio;
			float2 newSize = float2(mSize.x, mSize.y * ratio);
			mSize = newSize;
		}
	}
	else 
	{*/
		mSize = size; 
	//}

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
