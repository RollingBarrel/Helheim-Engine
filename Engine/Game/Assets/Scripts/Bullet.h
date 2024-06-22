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

	void Init(float3 position,float3 direction, float speed = 1.0f, float size = 1.0f);
	void OnCollisionEnter(CollisionData* collisionData);

private: 
	bool Delay(float delay);

	float mRange = 150.0f;
	float mSpeed = 0.1f;
	float3 mDirection = float3::zero;
	float mTimePassed = 0.0f;
	float mTotalMovement = 0.0f;
	BoxColliderComponent* mCollider = nullptr;
	bool mHasCollided = false;
	GameObject* mHitParticles = nullptr;


};
