#pragma once
#include <Script.h>
#include <vector>
#include "Macros.h"
#include "TimerScript.h"

struct CollisionData;
class BoxColliderComponent;
class GameObject;

enum class TRAP_STATE
{
	INACTIVE,
	EXPLOSION_PRESTART,
	EXPLOSION_START,
};

GENERATE_BODY(ExplosiveTrap);
class ExplosiveTrap :
	public Script
{
	FRIEND(ExplosiveTrap)
public:
	ExplosiveTrap(GameObject* owner);
	~ExplosiveTrap();
	void Update() override;
	void Start() override;
	void OnCollisionEnter(CollisionData* collisionData);

	void InnerTrapTakeDamage();

private:
	bool IsInTrap(const GameObject* target);

	BoxColliderComponent* mCollider = nullptr;
	BoxColliderComponent* mTrapCollider = nullptr; 

	std::vector<GameObject*> mInTrap;

	TRAP_STATE mState = TRAP_STATE::INACTIVE;

	float mTriggerArea = 2.0f;
	float mExplosionArea= 5.0f;

	GameObject* mExplosionPrestartVFX = nullptr;
	GameObject* mExplosionVFX = nullptr;

	// Activation
	float mExplosionWait = 1.5f;
	float mExplosionDuration = 4.0f;
	TimerScript mExplosionWaitTimer;
	TimerScript mExplosionDurationTimer;

	// Damage
	float mDamageAmount = 20.0f;
};

