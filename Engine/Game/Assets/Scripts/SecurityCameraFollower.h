#pragma once
#include <Script.h>
#include "Macros.h"

GENERATE_BODY(SecurityCameraFollower);

class SpotLightComponent;
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
	//void OnCollisionExit(CollisionData* collisionData);
	

private:

	float mMaxDistance = 15.0f;

	SpotLightComponent* mCameraLight = nullptr;
	float mMaxLightIntesity = 20.0f;
	float mMaxLightRange = 10.0f;

	float mTurningLightOnTimer = 0.0f;
	float mTurningLightOnTime = 5.0f;

	GameObject* mTarget = nullptr;
	BoxColliderComponent* mCollider = nullptr;

};
