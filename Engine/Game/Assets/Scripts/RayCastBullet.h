#pragma once
#include "Script.h"
#include "Macros.h"
#include "Physics.h"
#include "TimerScript.h"


class ParticleSystemComponent;
class TrailComponent;
class DecalComponent;
class ColorGradient;
struct CollisionData;
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

	void InitBulletholeDecal();

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
	ParticleSystemComponent* mDecalParticles = nullptr;
	bool mFadeDecal = false;
	float mDecalLifetime = 0;
	float mDecalFadingTime = 0;
	float mDecalPercentageWithParticles = 1.0f;

	TimerScript mHitTimer;

	//Trails WorkAround
	bool mFirstFrame = true;

};

