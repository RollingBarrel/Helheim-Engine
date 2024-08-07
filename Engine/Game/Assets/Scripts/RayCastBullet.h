#pragma once
#include "Script.h"
#include "Macros.h"
#include "Physics.h"
#include "TimerScript.h"


class ParticleSystemComponent;
class TrailComponent;
class ColorGradient;


GENERATE_BODY(RayCastBullet);
class RayCastBullet : public Script
{
	FRIEND(RayCastBullet)
public:
	RayCastBullet(GameObject* owner) : Script(owner) {}
	~RayCastBullet() {};

	void Update() override;

	void Init(const float3& startposition, Hit& hit, float damage, float speed, float size, const ColorGradient* gradient = nullptr);
	void Init(const float3& startposition, const float3& endPosition, float damage, float speed, float size, const ColorGradient* gradient = nullptr);
private:
	float mSpeed = 0.1f;
	float mDamage = 1.0f;
	bool mShooterIsPlayer = false;
	float3 mHitPoint = float3::zero;
	float3 mDirection = float3::zero;
	Hit mHit;
	ParticleSystemComponent* mHitParticles = nullptr;
	TrailComponent* mBulletTrail = nullptr;

	TimerScript mHitTimer;

	//Trails WorkAround
	bool mFirstFrame = true;

};

