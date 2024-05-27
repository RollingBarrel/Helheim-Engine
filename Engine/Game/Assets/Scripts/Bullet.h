#pragma once
#include "Script.h"
#include "Macros.h"
#include "float3.h"

class BoxColliderComponent;
class ParticleSystemComponent;
struct CollisionData;

GENERATE_BODY(Bullet);
class Bullet : public Script
{
	FRIEND(Bullet)
public:
	Bullet(GameObject* owner);
	~Bullet();
	void Start() override;
	void Update() override; 

	void OnCollisionEnter(CollisionData* collisionData);

	bool Delay(float delay);

	static unsigned int GetNumBullets() { return mNumBullets; }

private: 

	static unsigned int mNumBullets;

	float mRange = 15.0f;
	float mSpeed = 1.0f;
	float3 mDirection = float3::zero;
	float mTimePassed = 0.0f;
	float mTotalMovement = 0.0f;
	BoxColliderComponent* mCollider;
	bool mHasCollided = false;
	GameObject* mHitParticles = nullptr;
};
