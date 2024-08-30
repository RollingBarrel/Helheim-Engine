#pragma once
#include <Script.h>
#include <vector>
#include "Macros.h"
#include "TimerScript.h"

struct CollisionData;
class BoxColliderComponent;
class GameObject;

GENERATE_BODY(ExplosiveTrap);
class ExplosiveTrap : public Script
{
	FRIEND(ExplosiveTrap)
public:
	ExplosiveTrap(GameObject* owner);
	~ExplosiveTrap();
	void Update() override;
	void Start() override;
	void OnCollisionEnter(CollisionData* collisionData);

private:
	bool CheckIfCaptured(const GameObject* target);
	void ActiveTrap(bool active);

	BoxColliderComponent* mCollider = nullptr;
	std::vector<GameObject*> mInTrap;

	float mArea = 1.0f;
	GameObject* mTrigger = nullptr;

	// Activation
	bool mIsActive = false;

	float mTriggerDistance = 2.0f;
	float mExplosionWait = 0.75f;
	float mExplosionDuration = 3.0f;
	TimerScript mExplosionWaitTimer;
	TimerScript mExplosionDurationTimer;

	// Damage
	float mDamageAmount = 20.0f;
};

