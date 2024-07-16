#pragma once
#include "Enemy.h"

class GameObject;
class AnimationStateMachine;

GENERATE_BODY(EnemyCreatureRange);
class EnemyCreatureRange : public Enemy
{
	FRIEND(EnemyCreatureRange)

public:
	EnemyCreatureRange(GameObject* owner) : Enemy(owner) {}
	~EnemyCreatureRange() {}
	
	void Start() override;

private:
	void Attack() override;

	float mRangeDamage = 15.0f;
	float mAttackRotationSpeed = 10.0f;

	bool mDoDamage = false;

	GameObject* mLaserOrigin = nullptr;
	GameObject* mLaserTrail = nullptr;
	GameObject* mLaserEnd = nullptr;

	bool mMoveTrail = false;
};

