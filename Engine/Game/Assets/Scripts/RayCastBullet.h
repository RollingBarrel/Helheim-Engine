#pragma once
#include "Script.h"
#include "Macros.h"
#include "Physics.h"
#include "TimerScript.h"


class ParticleSystemComponent;
class TrailComponent;
class DecalComponent;
class ColorGradient;
class CollisionData;
class BoxColliderComponent;

GENERATE_BODY(RayCastBullet);
class RayCastBullet : public Script
{
	FRIEND(RayCastBullet)
public:
	RayCastBullet(GameObject* owner) : Script(owner) {}
	~RayCastBullet() {};

	void Start() override;
	void Update() override;

	void Init(const float3& startposition, Hit& hit, float damage, float speed, float size, const ColorGradient* gradient = nullptr);
	void Init(const float3& startposition, const float3& endPosition, float damage, float speed, float size, const ColorGradient* gradient = nullptr);

private:

	void SetDecalRotation();

	void OnCollisionEnter(CollisionData* collisionData);
	BoxColliderComponent* mCollider = nullptr;

	float mSpeed = 0.1f;
	float mDamage = 1.0f;
	bool mShooterIsPlayer = false;
	float3 mHitPoint = float3::zero;
	float3 mDirection = float3::zero;
	Hit mHit;
	ParticleSystemComponent* mHitParticles = nullptr;
	TrailComponent* mBulletTrail = nullptr;

	float3 mCollisionDirection = float3::zero;

	//Deacls
	DecalComponent* mHoleDecal = nullptr;
	bool mFadeDecal = false;
	float mDecalLifetime = 0;
	float mDecalFadingTime = 0;

	TimerScript mHitTimer;
	TimerScript mDelayDecalTimer;

	//Trails WorkAround
	bool mFirstFrame = true;

};

