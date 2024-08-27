#pragma once
#include "Enemy.h"
#include "Geometry/Ray.h"
#include "TimerScript.h"

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

private:
	//void Idle();
	void SelectAttack();
	void StartBulletAttack();
	void BulletAttack();
	void LaserAttack();
	void BombAttack();

	void  Death() override;

	float mBulletSpeed = 15.0f;
	int mLastAttack = -1;
	unsigned int mStage = 0;
	bool mBulletHell = false;

	//Collider
	BoxColliderComponent* mCollider = nullptr;

	TimerScript mPhaseShiftTimer;
	float mPhaseShiftTime = 5.0f;
	TimerScript mBulletHellTimer;
	float mBulletHellCooldown = 0.2f;

	const char* mTemplateNames[3] = { "BombingTemplate.prfb", "BombingTemplate1.prfb", "BombingTemplate2.prfb" };
	std::vector<GameObject*> mTemplates;
	GameObject* mLaserGO = nullptr;
	float mBulletHellAngleSpread = 120;
};

