#pragma once
#include <Script.h>
#include "Macros.h"

GENERATE_BODY(SecurityCameraFollower);

class Component;
class GameObject;
class BoxColliderComponent;

struct CollisionData;

class SecurityCameraFollower : public Script
{
	FRIEND(SecurityCameraFollower)

public:
	SecurityCameraFollower(GameObject* owner);
	
	void Start() override;
	void Update() override;

	void OnCollisionEnter(CollisionData* collisionData);
	void OnCollisionExit(CollisionData* collisionData);
	

private:

	Component* mCameraLight = nullptr;
	GameObject* mTarget = nullptr;
	BoxColliderComponent* mCollider = nullptr;


};
