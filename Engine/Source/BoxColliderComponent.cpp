#include "BoxColliderComponent.h"
#include "ScriptComponent.h"
#include "GameObject.h"
#include "Application.h"
#include "ModulePhysics.h"

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
	ComputeBoundingBox();
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
		ComputeBoundingBox();
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

void BoxColliderComponent::ComputeBoundingBox()
{
	float3 sizeIncrement = mSize * 0.5f;
	mLocalAABB = AABB(mCenter - sizeIncrement, mCenter + sizeIncrement);
	mWorldOBB = OBB(mLocalAABB);
	float3 position = mOwner->GetWorldPosition();
	Quat rotation = mOwner->GetWorldRotation();
	mWorldOBB.Transform(float4x4(rotation, position));
	App->GetPhysics()->UpdateBoxRigidbody(this);
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

	ComputeBoundingBox();
}

void BoxColliderComponent::Enable()
{
	App->GetPhysics()->EnableRigidbody(this);
	ComputeBoundingBox();
}

void BoxColliderComponent::Disable()
{
	App->GetPhysics()->DisableRigidbody(this);
}