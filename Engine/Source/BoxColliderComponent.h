#pragma once
#include "Component.h"
#include "ModulePhysics.h"

#include "Math/float3.h"
#include "Geometry/AABB.h"
#include "Geometry/OBB.h"
#include <functional>
#include <vector>

class GameObject;
class btRigidBody;
class MotionState;

enum class CollisionEventType : int
{
	ON_COLLISION_ENTER = 0,
	ON_COLLISION_EXIT,
	ON_DESTROY,
	COUNT
};

struct CollisionData
{
	CollisionEventType eventType;
	GameObject* collidedWith;
	const float3& collisionNormal;
	const float3& penetrationDistance;
};

class ENGINE_API BoxColliderComponent : public Component
{
public:
	BoxColliderComponent(GameObject* owner);
	BoxColliderComponent(const BoxColliderComponent& original, GameObject* owner);
	~BoxColliderComponent();

	void Init();
	void Reset() override;
	void Update() override;
	Component* Clone(GameObject* owner) const override;

	void AddCollisionEventHandler(CollisionEventType eventType, std::function<void(CollisionData*)> handler);
	void OnCollision(CollisionData* collisionData);
	void ComputeBoundingBox();

	inline const AABB& GetAABB() const { return mLocalAABB; }
	inline const OBB& GetOBB() const { return mWorldOBB; }
	inline const float3& GetCenter() const { return mCenter; }
	inline const float3& GetSize() const { return mSize; }
	inline const ColliderType GetColliderType() const { return mColliderType; }
	inline const bool GetFreezeRotation() const { return mFreezeRotation; }
	inline btRigidBody* GetRigidBody() const { return mRigidBody; }
	inline MotionState* GetMotionState() const { return mMotionState; }
	inline Collider GetCollider() const { return mCollider; }

	void SetCenter(const float3& center);
	void SetSize(const float3& size);
	void SetColliderType(ColliderType colliderType);
	void SetFreezeRotation(bool freezeRotation);
	void SetRigidBody(btRigidBody* rigidBody);
	void SetMotionState(MotionState* motionState);

	void Save(Archive& archive) const override;
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

private:
	AABB mLocalAABB = { float3(-0.5f), float3(0.5f) };
	OBB mWorldOBB = { mLocalAABB };
	float3 mCenter = float3::zero;
	float3 mSize = float3::one;
	ColliderType mColliderType = ColliderType::DYNAMIC;
	bool mFreezeRotation = false;

	btRigidBody* mRigidBody = nullptr;
	MotionState* mMotionState = nullptr;
	Collider mCollider{ this, typeid(Component) };

	std::vector<std::function<void(CollisionData*)>> mEventHandlers[(int)CollisionEventType::COUNT];

};

