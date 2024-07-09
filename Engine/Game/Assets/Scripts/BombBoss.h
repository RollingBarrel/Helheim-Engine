#pragma once
#include <Script.h>
#include "Macros.h"
#include "float3.h"

class BoxColliderComponent;
class ParticleSystemComponent;
struct CollisionData;

GENERATE_BODY(BombBoss);
class BombBoss : public Script
{
	FRIEND(BombBoss)
public:
	BombBoss(GameObject* owner);
	~BombBoss();

	void Start() override;
	void Update() override;

	void Init();

private:

	float mArea = 100.0f;
	float mDamage = 1.0f;
	float mTimeDelay = 3.0f;
	float mTimePassed = 0.0f;
	bool mHasExploded = false;
	std::vector<ParticleSystemComponent*> mExplosionParticles = nullptr;


};
