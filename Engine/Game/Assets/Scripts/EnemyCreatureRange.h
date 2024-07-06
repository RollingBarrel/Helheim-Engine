#pragma once
#include "Enemy.h"
#include "TimerScript.h"

class AnimationStateMachine;
struct CollisionData;
class BoxColliderComponent;
class GameObject;

enum class EnemyState
{
	IDLE,
	CHASE,
	ATTACK,
};

GENERATE_BODY(EnemyCreatureRange);
class EnemyCreatureRange : public Enemy
{
	FRIEND(EnemyCreatureRange)

public:
	EnemyCreatureRange(GameObject* owner);
	~EnemyCreatureRange() {}

	void Start() override;
	void Update() override;

private:
	void Idle();
	void Chase();
	void Attack();
	bool IsMoving();
	void Death() override;
	void Init() override;
	void RangeAttack();

	EnemyState mCurrentState = EnemyState::IDLE;
	TimerScript mDeathTimer;

	float mRangeDistance = 9.0f;
	float mRangeDamage = 15.0f;
	float mBulletSpeed = 0.65f;
	float mTimerAttack = 2.0f;
	float mAttackCD = 0.0f;
	float mTimerDisengage = 0.0f;

	GameObject* mBulletOrigin = nullptr;
	BoxColliderComponent* mCollider = nullptr;
};

