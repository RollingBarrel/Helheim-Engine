#pragma once
#include "Module.h"
#include "Math/float3.h"
#include <vector>

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btRigidBody;
class btMotionState;
class MotionState;
class BoxColliderComponent;
struct Collider;

enum class ColliderType : int
{
	DYNAMIC=0,
	STATIC,
	KINEMATIC,
	TRIGGER,
	COUNT
};

enum class WorldLayers : int
{
	COUNT
};

class ENGINE_API ModulePhysics : public Module
{
public:
	ModulePhysics();
	~ModulePhysics();

	bool Init() override;
	bool CleanUp() override;
	update_status PreUpdate(float dt) override;

	float GetGravity() { return mGravity; }
	void SetGravity(float gravity) { mGravity = gravity; }


	void CreateBoxRigidbody(BoxColliderComponent* boxCollider);
	void RemoveBoxRigidbody(BoxColliderComponent* boxCollider);
	void UpdateBoxRigidbody(BoxColliderComponent* boxCollider);

private:
	void AddBodyToWorld(btRigidBody* rigidbody, ColliderType colliderType, WorldLayers layer);
	btRigidBody* AddBoxBody(btMotionState* motionState, float3 size, float mass);

	void ProcessCollision(Collider* bodyA, Collider* bodyB, const float3& collisionNormal, const float3& diff);

	btDefaultCollisionConfiguration* mCollisionConfiguration = nullptr;
	btCollisionDispatcher* mDispatcher = nullptr;
	btBroadphaseInterface* mBroadPhase = nullptr;
	btSequentialImpulseConstraintSolver* mConstraintSolver = nullptr;
	btDiscreteDynamicsWorld* mWorld = nullptr;
	std::vector<btRigidBody*> mRigidBodiesToRemove;

	float mGravity = -9.81f;
};

