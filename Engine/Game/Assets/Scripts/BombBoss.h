#pragma once
#include <Script.h>
#include "Macros.h"
#include "float3.h"

class BoxColliderComponent;
class Component;
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

	void Init(float3 bombOrigin);

private:
	GameObject* mBombGO = nullptr;
	float mRadius = 2.5f;
	float mDamage = 1.0f;
	float mTimeDelay = 3.0f;
	float mTimePassed = 0.0f;
	float3 mBombOrigin = float3::zero;
	bool mHasExploded = false;
	std::vector<Component*> mExplosionParticles;


};
