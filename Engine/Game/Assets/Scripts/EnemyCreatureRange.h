#pragma once
#include "Enemy.h"

class AnimationStateMachine;
class GameObject;

GENERATE_BODY(EnemyCreatureRange);
class EnemyCreatureRange : public Enemy
{
	FRIEND(EnemyCreatureRange)

public:
	EnemyCreatureRange(GameObject* owner) : Enemy(owner) {}
	~EnemyCreatureRange() {}

private:
	void Attack() override;
	void RangeAttack();


	float mRangeDamage = 15.0f;
	float mBulletSpeed = 3.0f;

	GameObject* mLaserOrigin = nullptr;
};

