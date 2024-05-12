#include "BoxColliderComponent.h"
#include "GameObject.h"

BoxColliderComponent::BoxColliderComponent(GameObject* owner) : Component(owner, ComponentType::BOXCOLLIDER)
{
}

BoxColliderComponent::BoxColliderComponent(const BoxColliderComponent& original, GameObject* owner) : Component(owner, ComponentType::BOXCOLLIDER),
	mCenter(original.mCenter), mSize(original.mSize)
{
	ComputeBoundingBox();
}

BoxColliderComponent::~BoxColliderComponent()
{
}

void BoxColliderComponent::Reset()
{
	mCenter = float3::zero;
	mSize = float3::one;
}

Component* BoxColliderComponent::Clone(GameObject* owner) const
{
	return new BoxColliderComponent(*this, owner);
}

void BoxColliderComponent::OnCollision(GameObject* gameObject, const float3& collisionNormal, const float3& penetrationDistance)
{
}

void BoxColliderComponent::ComputeBoundingBox()
{
	float3 sizeIncrement = mSize * 0.5f;
	mLocalAABB = AABB(mCenter - sizeIncrement, mCenter + sizeIncrement);
	mWorldOBB = OBB(mLocalAABB);
	mWorldOBB.Transform(mOwner->GetWorldTransform()); // TODO: Check if correct
}

void BoxColliderComponent::SetCenter(const float3& center)
{
	mCenter = center;
	ComputeBoundingBox();
}

void BoxColliderComponent::SetSize(const float3& size)
{
	mSize = size;
	ComputeBoundingBox();
}

void BoxColliderComponent::Save(Archive& archive) const
{
	Component::Save(archive);
	archive.AddFloat3("Center", mCenter);
	archive.AddFloat3("Size", mSize);
}

void BoxColliderComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
	Component::LoadFromJSON(data, owner);
	if (data.HasMember("Center") && data["Center"].IsArray())
	{
		const rapidjson::Value& centerValues = data["Center"];
		float x{ 0.0f }, y{ 0.0f }, z{ 0.0f };
		if (centerValues.Size() == 3 && centerValues[0].IsFloat() && centerValues[1].IsFloat() && centerValues[2].IsFloat())
		{
			x = centerValues[0].GetFloat();
			y = centerValues[1].GetFloat();
			z = centerValues[2].GetFloat();
		}
		mCenter = float3(x, y, z);
	}

	if (data.HasMember("Size") && data["Size"].IsArray())
	{
		const rapidjson::Value& sizeValues = data["Size"];
		float x{ 0.0f }, y{ 0.0f }, z{ 0.0f };
		if (sizeValues.Size() == 3 && sizeValues[0].IsFloat() && sizeValues[1].IsFloat() && sizeValues[2].IsFloat())
		{
			x = sizeValues[0].GetFloat();
			y = sizeValues[1].GetFloat();
			z = sizeValues[2].GetFloat();
		}
		mSize = float3(x, y, z);
	}

	ComputeBoundingBox();
}
