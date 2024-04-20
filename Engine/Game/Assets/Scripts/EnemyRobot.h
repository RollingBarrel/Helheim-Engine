#pragma once
#include <Enemy.h>
#include "Geometry/Ray.h"
#include <ScriptComponent.h>

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
	void Start() override;
	void Update() override;

private:

	void Idle();
	void Chase();
	void Attack();

	void MeleeAttack() ;
	void RangeAttack();

	EnemyState mCurrentState = EnemyState::IDLE;
	RobotType mType = RobotType::RANGE;

	float mRangeDistance = 9.0f;
	float mRangeDamage = 15.0f;

	float mMeleeDistance = 2.0f;
	float mMeeleDamage = 10.0f;
	

};

