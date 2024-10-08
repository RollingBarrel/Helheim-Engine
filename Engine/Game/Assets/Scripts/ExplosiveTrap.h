#pragma once
#include <Script.h>
#include "Macros.h"
#include "TimerScript.h"

struct CollisionData;
class BoxColliderComponent;
class GameObject;

enum class TRAP_STATE
{
	INACTIVE,
	EXPLOSION_CHARGE,
	EXPLOSION_START,
	EXPLOSION_END
};

GENERATE_BODY(ExplosiveTrap);
class ExplosiveTrap :
	public Script
{
	FRIEND(ExplosiveTrap)
public:
	ExplosiveTrap(GameObject* owner) : Script(owner) {}
	~ExplosiveTrap() {}
	void Update() override;
	void Start() override;
	void OnCollisionEnter(CollisionData* collisionData);

	void InnerTrapTakeDamage();

private:
	void DoDamage();

	BoxColliderComponent* mCollider = nullptr;
	BoxColliderComponent* mTrapCollider = nullptr; 

	TRAP_STATE mState = TRAP_STATE::INACTIVE;

	float mTriggerArea = 2.0f;
	float mExplosionArea= 5.0f;

	GameObject* mExplosionPrestartVFX = nullptr;
	GameObject* mExplosionVFX = nullptr;
	GameObject* mBarrelMesh = nullptr;

	// Activation
	float mExplosionWait = 1.5f;
	float mExplosionDuration = 4.0f;
	TimerScript mExplosionChargeTimer;
	TimerScript mExplosionDurationTimer;

	// Damage
	float mDamageAmount = 20.0f;
};

