#pragma once
#include "Enemy.h"

struct CollisionData;
class BoxColliderComponent;

GENERATE_BODY(EnemyBoss);
class EnemyBoss : public Enemy
{
	FRIEND(EnemyBoss)

public:
	EnemyBoss(GameObject* owner);
	~EnemyBoss() {}
	void Update() override;
	void Start() override;
	float GetBombsDelay() const { return mBombsDelay; }

private:
	//void Idle();
	void SelectAttack();
	void StartBulletAttack();
	void BulletAttack();
	void LaserAttack();
	void BombAttack();

	void  Death() override;

	float mBulletSpeed = 15.0f;
	float mBulletsDamage = 2.0f;
	float mLaserDamage = 2.0f;
	float mBombDamage = 5.0f;
	int mLastAttack = -1;
	unsigned int mStage = 0;
	bool mBulletHell = false;

	//Collider
	BoxColliderComponent* mCollider = nullptr;

	TimerScript mPhaseShiftTimer;
	float mPhaseShiftTime = 5.0f;
	TimerScript mBulletHellTimer;
	float mBulletHellCooldown = 0.2f;
	float mBulletHellDuration = 6.0f;

	//Laser
	float mLaserDuration = 5.0f;
	float mLaserDamage = 3.0f;
	float mLaserSpeed = 2.0f;
	float mLaserDistance = 10.0f;
	float mBombsDelay = 1.f;
	float mBombsDuration = 2.0f;

	const char* mTemplateNames[6] = { "BombingTemplate.prfb", "BombingTemplate1.prfb", "BombingTemplate2.prfb", "BombingTemplate3.prfb", "BombingTemplate4.prfb", "BombingTemplate5.prfb" };
	std::vector<GameObject*> mTemplates;
	GameObject* mLaserGO = nullptr;
	float mBulletHellAngleSpread = 180;
};

