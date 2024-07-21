#pragma once
#include "Enemy.h"
#include "Geometry/Ray.h"
#include "TimerScript.h"

struct CollisionData;
class BoxColliderComponent;

enum class BossState
{
	IDLE,
	ATTACK,
};



GENERATE_BODY(EnemyBoss);
class EnemyBoss : public Enemy
{
	FRIEND(EnemyBoss)

public:
	EnemyBoss(GameObject* owner);
	~EnemyBoss() {}
	void Update() override;
	void Start() override;

private:
	void Idle();
	void Attack();
	void BulletAttack();
	void LaserAttack();
	void BombAttack();

	void  Death() override;

	BossState mCurrentState = BossState::IDLE;

	float mRangeDistance = 9.0f;
	float mRangeDamage = 15.0f;
	float mBulletSpeed = 0.65f;
	float mTimerAttack = 2.0f;
	float mAttackCD = 0.0f;
	int mLastAttack = -1;

	//Collider
	BoxColliderComponent* mCollider = nullptr;

	const const char* mTemplateNames[3] = { "BombingTemplate.prfb", "BombingTemplate1.prfb", "BombingTemplate2.prfb" };
	std::vector<GameObject*> mTemplates;
	GameObject* mLaserGO = nullptr;
};

