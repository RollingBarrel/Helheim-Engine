#pragma once
#include "Enemy.h"
#include "Geometry/Ray.h"
#include "TimerScript.h"

struct CollisionData;
class BoxColliderComponent;

enum class BossState
{
	IDLE,
	ATTACK,
};



GENERATE_BODY(EnemyBoss);
class EnemyBoss : public Enemy
{
	FRIEND(EnemyBoss)

public:
	EnemyBoss(GameObject* owner);
	~EnemyBoss() {}
	void Update() override;
	void Start() override;

private:
	void Idle();
	void Attack();
	void BulletAttack();
	void LaserAttack();
	void BombAttack();

	void MeleeAttack();
	void  Death() override;
	void Reset() override;
	void OnCollisionEnter(CollisionData* collisionData);

	void PlayStepAudio();
	void PlayMeleeAudio();

	BossState mCurrentState = BossState::IDLE;

	TimerScript mDeathTimer;

	float mRangeDistance = 9.0f;
	float mRangeDamage = 15.0f;

	float mMeleeDistance = 2.0f;
	float mMeeleDamage = 10.0f;
	float mMeleeAttackCoolDown = 2.0f;
	float mTimerAttack = 0.0f;
	float mTimerDisengage = 0.0f;

	//Collider
	BoxColliderComponent* mCollider = nullptr;

	// MoveSimulation
	float mStepTimer = 0.0f;
	float mStepCooldown = 0.5f;
};

