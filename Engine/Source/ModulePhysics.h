#pragma once
#include "Module.h"
#include "Math/float3.h"

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btRigidBody;
struct Collider;

class ENGINE_API ModulePhysics : public Module
{
public:
	ModulePhysics();
	~ModulePhysics();

	bool Init() override;
	update_status PreUpdate(float dt) override;

	float GetGravity() { return mGravity; }
	void SetGravity(float gravity) { mGravity = gravity; }

private:
	void ProcessCollision(Collider* bodyA, Collider* bodyB, const float3& collisionNormal, const float3& diff);

	btDefaultCollisionConfiguration* collisionConfiguration = nullptr;
	btCollisionDispatcher* dispatcher = nullptr;
	btBroadphaseInterface* broadPhase = nullptr;
	btSequentialImpulseConstraintSolver* constraintSolver = nullptr;
	btDiscreteDynamicsWorld* world = nullptr;

	float mGravity = -9.81f;
};

