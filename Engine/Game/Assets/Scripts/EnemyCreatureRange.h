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
	void Update() override;
	void Charge() override;
	void Attack() override;

private:
	void Rotate();

	float mAttackRotationSpeed = 10.0f;
	bool mDoDamage = false;

	GameObject* mLaserOrigin = nullptr;
	GameObject* mLaserTrail = nullptr;
	GameObject* mLaserEnd = nullptr;
	GameObject* mLaserCharge = nullptr;

	//Laser Trail WorkAround
	bool mMoveTrail = false;

};

