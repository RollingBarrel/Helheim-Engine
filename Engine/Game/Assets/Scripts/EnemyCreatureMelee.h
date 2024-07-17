#pragma once
#include "Enemy.h"
#include "TimerScript.h"

struct CollisionData;
class BoxColliderComponent;

enum class EnemyState
{
	IDLE,
	CHASE,
	ATTACK,
};

GENERATE_BODY(EnemyCreatureMelee);
class EnemyCreatureMelee : public Enemy
{
	FRIEND(EnemyCreatureMelee)

public:
	EnemyCreatureMelee(GameObject* owner);
	~EnemyCreatureMelee() {}
	
	void Start() override;
	void Update() override;

private:
	void Idle();
	void Chase();
	void Attack();
	bool IsMoving();

	void MeleeAttack();
	void Death() override;
	void Init() override;
	void OnCollisionEnter(CollisionData* collisionData);

	void PlayStepAudio();
	void PlayMeleeAudio();

	EnemyState mCurrentState = EnemyState::IDLE;

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

