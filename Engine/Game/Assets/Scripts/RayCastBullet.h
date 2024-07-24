#pragma once
#include "Script.h"
#include "Macros.h"
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

	void Init(const float3& startposition, const float3& endPosition, float speed, float size, bool hit, ColorGradient* gradient = nullptr);

private:
	float mSpeed = 0.1f;
	bool mShooterIsPlayer = false;
	float3 mHitPoint = float3::zero;
	float3 mDirection = float3::zero;
	bool mHit = false;
	ParticleSystemComponent* mHitParticles = nullptr;
	TrailComponent* mBulletTrail = nullptr;

	TimerScript mHitTimer;

};

