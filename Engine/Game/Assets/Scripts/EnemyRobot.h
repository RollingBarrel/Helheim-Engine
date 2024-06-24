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

enum class RobotType
{
	RANGE,
	MELEE
};

GENERATE_BODY(EnemyRobot);
class EnemyRobot : public Enemy
{
	FRIEND(EnemyRobot)

public:
	EnemyRobot(GameObject* owner);
	~EnemyRobot() {}
	void Update() override;
	void Start() override;
	
private:
	void Idle();
	void Chase();
	void Attack();
	bool IsMoving();

	void MeleeAttack();
	void RangeAttack();

	void PlayStepAudio();
	void PlayMeleeAudio();

	EnemyState mCurrentState = EnemyState::IDLE;
	RobotType mType = RobotType::MELEE;

	AnimationStateMachine* mStateMachine = nullptr;

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

