#pragma once
#include "Enemy.h"
#include "Geometry/Ray.h"

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
	void  Death() override;
	void RangeAttack();

	EnemyState mCurrentState = EnemyState::IDLE;


	float mRangeDistance = 9.0f;
	float mRangeDamage = 15.0f;

	float mTimerAttack = 2.0f;
	float mAttackCD = 0.0f;
	float mTimerDisengage = 0.0f;
	GameObject* mBulletOrigin = nullptr;
	//Collider
	BoxColliderComponent* mCollider = nullptr;
};

