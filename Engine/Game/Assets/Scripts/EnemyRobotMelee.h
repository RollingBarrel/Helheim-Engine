#pragma once
#include "Enemy.h"
#include "Geometry/Ray.h"

struct CollisionData;
class BoxColliderComponent;

enum class EnemyState
{
	IDLE,
	CHASE,
	ATTACK,
};



GENERATE_BODY(EnemyRobotMelee);
class EnemyRobotMelee : public Enemy
{
	FRIEND(EnemyRobotMelee)

public:
	EnemyRobotMelee(GameObject* owner);
	~EnemyRobotMelee() {}
	void Update() override;
	void Start() override;

private:
	void Idle();
	void Chase();
	void Attack();
	bool IsMoving();

	void MeleeAttack();
	void  Death() override;
	void OnCollisionEnter(CollisionData* collisionData);

	EnemyState mCurrentState = EnemyState::IDLE;


	float mRangeDistance = 9.0f;
	float mRangeDamage = 15.0f;

	float mMeleeDistance = 2.0f;
	float mMeeleDamage = 10.0f;
	float mMeleeAttackCoolDown = 2.0f;
	float mTimerAttack = 0.0f;
	float mTimerDisengage = 0.0f;

	//Collider
	BoxColliderComponent* mCollider = nullptr;
};

