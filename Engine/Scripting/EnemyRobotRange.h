#pragma once
#include "Enemy.h"
#include "Geometry/Ray.h"

class AnimationStateMachine;
struct CollisionData;
class BoxColliderComponent;

enum class EnemyState
{
	IDLE,
	CHASE,
	ATTACK,
};

GENERATE_BODY(EnemyRobotRange);
class EnemyRobotRange : public Enemy
{
	FRIEND(EnemyRobotRange)

public:
	EnemyRobotRange(GameObject* owner);
	~EnemyRobotRange() {}
	void Update() override;
	void Start() override;

private:
	void Idle();
	void Chase();
	void Attack();
	bool IsMoving();

	void RangeAttack();

	EnemyState mCurrentState = EnemyState::IDLE;

	AnimationStateMachine* mStateMachine = nullptr;

	float mRangeDistance = 9.0f;
	float mRangeDamage = 15.0f;

	float mTimerAttack = 0.0f;
	float mTimerDisengage = 0.0f;

	//Collider
	BoxColliderComponent* mCollider = nullptr;
};

