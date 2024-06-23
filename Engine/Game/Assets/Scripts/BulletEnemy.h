#pragma once
#include <Script.h>
#include "Macros.h"
#include "float3.h"

class BoxColliderComponent;
class ParticleSystemComponent;
struct CollisionData;

GENERATE_BODY(BulletEnemy);
class BulletEnemy : public Script
{
	FRIEND(BulletEnemy)
public:
	BulletEnemy(GameObject* owner);
	~BulletEnemy();

	void Start() override;
	void Update() override;

	void Init();
	void OnCollisionEnter(CollisionData* collisionData);

private:
	bool Delay(float delay);

	float mRange = 100.0f;
	float mSpeed = 0.1f;
	float mDamage = 1.0f;
	float3 mDirection = float3::zero;
	float mTimePassed = 0.0f;
	float mTotalMovement = 0.0f;
	BoxColliderComponent* mCollider = nullptr;
	bool mHasCollided = false;
	ParticleSystemComponent* mHitParticles = nullptr;


};
