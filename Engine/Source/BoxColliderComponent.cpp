#include "BoxColliderComponent.h"
#include "ScriptComponent.h"
#include "GameObject.h"
#include "Application.h"
#include "ModulePhysics.h"

BoxColliderComponent::BoxColliderComponent(GameObject* owner) : Component(owner, ComponentType::BOXCOLLIDER)
{
	Init();
}

BoxColliderComponent::BoxColliderComponent(const BoxColliderComponent& original, GameObject* owner) : Component(owner, ComponentType::BOXCOLLIDER),
	mCenter(original.mCenter), mSize(original.mSize)
{
	Init();
}

BoxColliderComponent::~BoxColliderComponent()
{
	if (mRigidBody)
	{
		App->GetPhysics()->RemoveBoxRigidbody(this);
	}
}

void BoxColliderComponent::Init()
{
	if (!mRigidBody && IsEnabled() && mOwner->IsActive())
	{
		App->GetPhysics()->CreateBoxRigidbody(this);
	}

	ComputeBoundingBox();
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

void BoxColliderComponent::OnCollision(GameObject* collidedWith, const float3& collisionNormal, const float3& penetrationDistance)
{
	for (Component* component : mOwner->GetComponents(ComponentType::SCRIPT))
	{
		if (component != nullptr)
		{
			ScriptComponent* scriptComponent = static_cast<ScriptComponent*>(component);
			Script* script = scriptComponent->GetScriptInstance();
			if (script != nullptr)
			{
				script->OnCollision(collidedWith, collisionNormal, penetrationDistance);
			}
		}
	}
}

void BoxColliderComponent::ComputeBoundingBox()
{
	float3 sizeIncrement = mSize * 0.5f;
	mLocalAABB = AABB(mCenter - sizeIncrement, mCenter + sizeIncrement);
	mWorldOBB = OBB(mLocalAABB);
	mWorldOBB.Transform(mOwner->GetWorldTransform());
}

void BoxColliderComponent::SetCenter(const float3& center)
{
	mCenter = center;
	ComputeBoundingBox();
	App->GetPhysics()->UpdateBoxRigidbody(this);
}

void BoxColliderComponent::SetSize(const float3& size)
{
	mSize = size;
	ComputeBoundingBox();
	App->GetPhysics()->UpdateBoxRigidbody(this);
}

void BoxColliderComponent::SetColliderType(ColliderType colliderType)
{
	mColliderType = colliderType;
	App->GetPhysics()->UpdateBoxRigidbody(this);
}

void BoxColliderComponent::SetFreezeRotation(bool freezeRotation)
{
	mFreezeRotation = freezeRotation;
	App->GetPhysics()->UpdateBoxRigidbody(this);
}

void BoxColliderComponent::SetRigidBody(btRigidBody* rigidBody)
{
	mRigidBody = rigidBody;
	App->GetPhysics()->UpdateBoxRigidbody(this);
}

void BoxColliderComponent::SetMotionState(MotionState* motionState)
{
	mMotionState = motionState;
	App->GetPhysics()->UpdateBoxRigidbody(this);
}

void BoxColliderComponent::Save(Archive& archive) const
{
	Component::Save(archive);
	archive.AddFloat3("Center", mCenter);
	archive.AddFloat3("Size", mSize);
	archive.AddInt("ColliderType", static_cast<int>(mColliderType));
	archive.AddBool("FreezeRotation", mFreezeRotation);
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
	if (data.HasMember("ColliderType") && data["ColliderType"].IsInt())
	{
		mColliderType = static_cast<ColliderType>(data["ColliderType"].GetInt());
	}
	if (data.HasMember("FreezeRotation") && data["FreezeRotation"].IsBool())
	{
		mFreezeRotation = data["FreezeRotation"].GetBool();
	}

	ComputeBoundingBox();
}
