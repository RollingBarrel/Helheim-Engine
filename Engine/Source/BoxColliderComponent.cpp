#include "BoxColliderComponent.h"
#include "BoxColliderComponent.h"
#include "ScriptComponent.h"
#include "GameObject.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "MotionState.h"
#include "btBulletDynamicsCommon.h"
#include "Geometry/OBB.h"

BoxColliderComponent::BoxColliderComponent(GameObject* owner) : Component(owner, ComponentType::BOXCOLLIDER)
{
	mCollider = new Collider(this, ComponentType::BOXCOLLIDER );
	Init();
}

BoxColliderComponent::BoxColliderComponent(const BoxColliderComponent& original, GameObject* owner) : Component(owner, ComponentType::BOXCOLLIDER),
	mCenter(original.mCenter), mSize(original.mSize)
{
	mCollider = new Collider(this, ComponentType::BOXCOLLIDER);
	Init();
}

BoxColliderComponent::~BoxColliderComponent()
{
	if (mRigidBody)
	{
		App->GetPhysics()->RemoveBoxRigidbody(this);
		mRigidBody = nullptr;
	}
	delete mCollider;
}

void BoxColliderComponent::Init()
{
	App->GetPhysics()->CreateBoxRigidbody(this);
	App->GetPhysics()->UpdateBoxRigidbody(this);
}

void BoxColliderComponent::Reset()
{
	mCenter = float3::zero;
	mSize = float3::one;
}

void BoxColliderComponent::Update()
{
	if (mOwner->HasUpdatedTransform())
	{
		App->GetPhysics()->UpdateBoxRigidbody(this);
	}
}

Component* BoxColliderComponent::Clone(GameObject* owner) const
{
	return new BoxColliderComponent(*this, owner);
}

void BoxColliderComponent::AddCollisionEventHandler(CollisionEventType eventType, std::function<void(CollisionData*)>* handler)
{
	mEventHandlers[static_cast<int>(eventType)].push_back(handler);
}

void BoxColliderComponent::OnCollision(CollisionData* collisionData)
{
	for (auto& handler : mEventHandlers[static_cast<int>(collisionData->eventType)])
	{
		(*handler)(collisionData);
	}
}

void BoxColliderComponent::GetColliderOBB(OBB& obb) const
{
	btVector3 aabbMinBullet;
	btVector3 aabbMaxBullet;
	mRigidBody->getAabb(aabbMinBullet, aabbMaxBullet);
	float3 aabbMin(aabbMinBullet.x(), aabbMinBullet.y(), aabbMinBullet.z());
	float3 aabbMax(aabbMaxBullet.x(), aabbMaxBullet.y(), aabbMaxBullet.z());

	obb.pos = (aabbMin + aabbMax) / 2.0f;
	obb.axis[0] = mOwner->GetRight();
	obb.axis[1] = mOwner->GetUp();
	obb.axis[2] = mOwner->GetFront();
	obb.r = mSize;
}

void BoxColliderComponent::SetCenter(const float3& center)
{
	mCenter = center;
	App->GetPhysics()->UpdateBoxRigidbody(this);
}

void BoxColliderComponent::SetSize(const float3& size)
{
	mSize = size;
	for (int i = 0; i < 3; ++i)
	{
		if (mSize[i] <= 0.001f && mSize[i] >= -0.001f)
		{
			mSize[i] = 0.001f;
		}
	}
	App->GetPhysics()->UpdateBoxRigidbody(this);
}

void BoxColliderComponent::SetColliderType(ColliderType colliderType)
{
	mColliderType = colliderType;
}

void BoxColliderComponent::SetFreezeRotation(bool freezeRotation)
{
	mFreezeRotation = freezeRotation;
}

void BoxColliderComponent::SetRigidBody(btRigidBody* rigidBody)
{
	mRigidBody = rigidBody;
}

void BoxColliderComponent::SetMotionState(MotionState* motionState)
{
	mMotionState = motionState;
}

void BoxColliderComponent::Save(JsonObject& obj) const
{
	Component::Save(obj);

	obj.AddFloats("Center", mCenter.ptr(), 3);
	obj.AddFloats("Size", mSize.ptr(), 3);
	obj.AddInt("ColliderType", static_cast<int>(mColliderType));
	obj.AddBool("FreezeRotation", mFreezeRotation);
}

void BoxColliderComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
	Component::Load(data, uidPointerMap);

	float center[3];
	data.GetFloats("Center", center);
	mCenter = float3(center);

	float size[3];
	data.GetFloats("Size", size);
	mSize = float3(size);

	mColliderType = (ColliderType)data.GetInt("ColliderType");
	mFreezeRotation = data.GetBool("FreezeRotation");

	App->GetPhysics()->UpdateBoxRigidbody(this);
}

void BoxColliderComponent::Enable()
{
	App->GetPhysics()->EnableRigidbody(this);
	App->GetPhysics()->UpdateBoxRigidbody(this);
}

void BoxColliderComponent::Disable()
{
	App->GetPhysics()->DisableRigidbody(this);
}