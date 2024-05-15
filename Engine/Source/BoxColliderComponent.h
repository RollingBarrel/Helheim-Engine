#pragma once
#include "Component.h"

#include "Math/float3.h"
#include "Geometry/AABB.h"
#include "Geometry/OBB.h"

class GameObject;
class btRigidBody;
class MotionState;

class ENGINE_API BoxColliderComponent : public Component
{
public:
	BoxColliderComponent(GameObject* owner);
	BoxColliderComponent(const BoxColliderComponent& original, GameObject* owner);
	~BoxColliderComponent();

	void Init();
	void Reset() override;
	void Update() override { ComputeBoundingBox(); } // TODO: Better only when transform is modified
	Component* Clone(GameObject* owner) const override;

	void OnCollision(GameObject* collidedWith, const float3& collisionNormal, const float3& penetrationDistance);
	void ComputeBoundingBox();

	inline const AABB& GetAABB() const { return mLocalAABB; }
	inline const OBB& GetOBB() const { return mWorldOBB; }
	inline const float3& GetCenter() const { return mCenter; }
	inline const float3& GetSize() const { return mSize; }
	inline const bool GetFreezeRotation() const { return mFreezeRotation; }
	inline btRigidBody* GetRigidBody() const { return mRigidBody; }
	inline MotionState* GetMotionState() const { return mMotionState; }

	void SetCenter(const float3& center);
	void SetSize(const float3& size);
	void SetFreezeRotation(bool freezeRotation) { mFreezeRotation = freezeRotation; }
	void SetRigidBody(btRigidBody* rigidBody) { mRigidBody = rigidBody; /*rigidBody->setUserPointer(collider);*/ }
	void SetMotionState(MotionState* motionState) { mMotionState = motionState; }

	void Save(Archive& archive) const override;
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

private:
	AABB mLocalAABB = { float3(-0.5f), float3(0.5f) };
	OBB mWorldOBB = { mLocalAABB };
	float3 mCenter = float3::zero;
	float3 mSize = float3::one;
	bool mFreezeRotation = false;

	btRigidBody* mRigidBody = nullptr;
	MotionState* mMotionState = nullptr;
	//Collider collider;

};

