#pragma once
#include "Enemy.h"
#include "Geometry/Ray.h"

class AnimationStateMachine;
class GameObject;

GENERATE_BODY(EnemyRobotRange);
class EnemyRobotRange : public Enemy
{
	FRIEND(EnemyRobotRange)

public:
	EnemyRobotRange(GameObject* owner) : Enemy(owner) {}
	~EnemyRobotRange() {}

private:
	void Attack() override;
	void RangeAttack();

	float mRangeDamage = 15.0f;
	float mBulletSpeed = 3.0f;
	float mTimerDisengage = 0.0f;

	GameObject* mBulletOrigin = nullptr;
};

