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

	float3 mLookingAtLocation = float3::zero;

	float mMaxDistance = 15.0f;
	float mSpeed = 3.0f;

	SpotLightComponent* mCameraLight = nullptr;
	float mMaxLightIntesity = 20.0f;
	float mMaxLightRange = 10.0f;

	float mTurningLightOnTimer = 0.0f;
	float mTurningOnOffTime = 3.0f;
	bool mOutOfReach = true;

	GameObject* mTarget = nullptr;
	BoxColliderComponent* mCollider = nullptr;

};
