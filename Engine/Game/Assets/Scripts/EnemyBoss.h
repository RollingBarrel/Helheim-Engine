#pragma once
#include "Enemy.h"

struct CollisionData;
class BoxColliderComponent;

enum BulletPattern
{
	CIRCLES,
	ARROW,
	WAVE,
	TARGETED_CIRCLES,
	NONE
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
	float GetBombsDelay() const { return mBombsDelay; }
	void LookAt(float3 target);

private:
	//void Idle();
	void UpdatePhase1();
	void UpdatePhase2();
	void UpdatePhase3();
	void StartBulletAttack(BulletPattern pattern);
	void BulletHellPattern1();
	void BulletHellPattern2();
	void BulletHellPattern3();
	void BulletHellPattern4();
	void BulletHellPattern5();
	void BulletHellPattern6();
	void LaserAttack();
	void BombAttack();	

	void  Death() override;

	int mLastAttack = -1;
	float3 mFront = float3::unitX;
	float3 mTargetFront = float3::unitX;
	unsigned int mStage = 0;
	float mPhase1Hp = 0.6f;
	float mPhase2Hp = 0.3f;

	//Collider
	BoxColliderComponent* mCollider = nullptr;

	TimerScript mPhaseShiftTimer;
	float mPhaseShiftTime = 5.0f;
	TimerScript mBulletHellTimer;
	
	float mAttackSequenceCooldown = 4.0f;

	// Bullet Hell
	float mBulletHellDuration = 5.0f;
	float mBulletSpeed = 15.0f;
	float mBulletsDamage = 2.0f;
	float mBulletRange = 50.0f;
	unsigned int mBulletsWave = 0;
	BulletPattern mBulletHell = BulletPattern::NONE;

	//Laser
	float mLaserDuration = 5.0f;
	float mLaserDamage = 3.0f;
	float mLaserSpeed = 2.0f;
	float mLaserDistance = 10.0f;
	float mBombsDelay = 1.f;
	float mBombsDuration = 2.0f;

	//Bombs
	float mBombDamage = 5.0f;

	const char* mTemplateNames[6] = { "BombingTemplate.prfb", "BombingTemplate1.prfb", "BombingTemplate2.prfb", "BombingTemplate3.prfb", "BombingTemplate4.prfb", "BombingTemplate5.prfb" };
	std::vector<GameObject*> mTemplates;
	GameObject* mLaserGO = nullptr;
};

