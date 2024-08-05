#pragma once
#include <Script.h>
#include "Macros.h"
#include "float3.h"

class BoxColliderComponent;
class GameObject;
struct CollisionData;

GENERATE_BODY(BossLaser);
class BossLaser : public Script
{
	FRIEND(BossLaser)
public:
	BossLaser(GameObject* owner);
	~BossLaser();

	void Start() override;
	void Update() override;

	void Init(float damage, float range);

	void OnCollisionEnter(CollisionData* collisionData);

private:
	BoxColliderComponent* mCollider = nullptr;
	float mDamage = 5.0f;
	float mAngle = 90.0f;
	float mSpeed = 25.0f;
	float mRange = 10.0f;
	unsigned int mIframes = 0;
	float mSwipeProgress = 0.0f;

	//Gameobject
	GameObject* mLaserOrigin = nullptr;
	GameObject* mLaserTrail = nullptr;
	GameObject* mLaserEnd = nullptr;
	GameObject* mLaserCharge = nullptr;


	//Laser WorkAround
	bool mMoveTrail = false;

};
