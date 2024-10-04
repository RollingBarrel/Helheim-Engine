#pragma once
#include "Enemy.h"

class GameObject;

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
	void Death() override;

	void TakeDamage(float damage) override;

private:
	
	void LaserCollide(GameObject* origin, GameObject* end, bool dealDamage); // false when is preview
	float mAttackRotationSpeed = 10.0f;
	bool mDoDamage = false;

	GameObject* mLaserOrigin = nullptr;
	GameObject* mLaserEnd = nullptr;
	GameObject* mLaserCharge = nullptr;
	GameObject* mPreviewOrigin = nullptr;
	GameObject* mPreviewEnd = nullptr;


	float mAimTime = 0.0f;
	TimerScript mAimTimer;

	//Laser Trail WorkAround
	bool mMoveTrail = false;

	int mLaserSound = -1;
};

