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
	obj.AddFloats("Translation", mPosition.ptr(), 3);
	obj.AddFloats("Rotation", mRotation.ptr(), 4);
	obj.AddFloats("Size", mSize.ptr(), 2);
	obj.AddFloats("AnchorMin", mAnchorMin.ptr(), 2);
	obj.AddFloats("AnchorMax", mAnchorMax.ptr(), 2);
	obj.AddFloats("Pivot", mPivot.ptr(), 2);
}

void Transform2DComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
	Component::LoadFromJSON(data, owner);

	if (data.HasMember("Translation") && data["Translation"].IsArray()) 
	{
		const rapidjson::Value& translationValues = data["Translation"];
		float x{ 0.0f }, y{ 0.0f }, z{ 0.0f };
		if (translationValues.Size() == 3 && translationValues[0].IsFloat() && translationValues[1].IsFloat() && translationValues[2].IsFloat()) 
		{
			x = translationValues[0].GetFloat();
			y = translationValues[1].GetFloat();
			z = translationValues[2].GetFloat();
		}

		mPosition = float3(x, y, z);
	}
	if (data.HasMember("Rotation") && data["Rotation"].IsArray()) 
	{
		const rapidjson::Value& rotationValues = data["Rotation"];
		float x{ 0.0f }, y{ 0.0f }, z{ 0.0f }, w{ 0.0f };
		if (rotationValues.Size() == 4 && rotationValues[0].IsFloat() && rotationValues[1].IsFloat() && rotationValues[2].IsFloat() && rotationValues[3].IsFloat()) 
		{
			x = rotationValues[0].GetFloat();
			y = rotationValues[1].GetFloat();
			z = rotationValues[2].GetFloat();
			w = rotationValues[3].GetFloat();
		}

		mRotation = Quat(x, y, z, w);
	}
	if (data.HasMember("Size") && data["Size"].IsArray()) 
	{
		const rapidjson::Value& sizeValues = data["Size"];
		float x{ 0.0f }, y{ 0.0f }, z{ 0.0f };
		if (sizeValues.Size() == 2 && sizeValues[0].IsFloat() && sizeValues[1].IsFloat()) 
		{
			x = sizeValues[0].GetFloat();
			y = sizeValues[1].GetFloat();
		}

		mSize = float2(x, y);
	}
	if (data.HasMember("AnchorMin") && data["AnchorMin"].IsArray()) 
	{
		const rapidjson::Value& anchorMinValues = data["AnchorMin"];
		float x{ 0.0f }, y{ 0.0f }, z{ 0.0f };
		if (anchorMinValues.Size() == 2 && anchorMinValues[0].IsFloat() && anchorMinValues[1].IsFloat()) 
		{
			x = anchorMinValues[0].GetFloat();
			y = anchorMinValues[1].GetFloat();
		}

		mAnchorMin = float2(x, y);
	}
	if (data.HasMember("AnchorMax") && data["AnchorMax"].IsArray()) 
	{
		const rapidjson::Value& anchorMaxValues = data["AnchorMax"];
		float x{ 0.0f }, y{ 0.0f }, z{ 0.0f };
		if (anchorMaxValues.Size() == 2 && anchorMaxValues[0].IsFloat() && anchorMaxValues[1].IsFloat()) 
		{
			x = anchorMaxValues[0].GetFloat();
			y = anchorMaxValues[1].GetFloat();
		}

		mAnchorMax = float2(x, y);
	}
	if (data.HasMember("Pivot") && data["Pivot"].IsArray()) 
	{
		const rapidjson::Value& pivotValues = data["Pivot"];
		float x{ 0.0f }, y{ 0.0f }, z{ 0.0f };
		if (pivotValues.Size() == 2 && pivotValues[0].IsFloat() && pivotValues[1].IsFloat()) 
		{
			x = pivotValues[0].GetFloat();
			y = pivotValues[1].GetFloat();
		}

		mPivot = float2(x, y);
	}

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
