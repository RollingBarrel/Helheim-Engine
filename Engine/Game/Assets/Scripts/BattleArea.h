#pragma once
#include <Script.h>
#include "Macros.h"

struct CollisionData;
class EnemySpawner;
class BoxColliderComponent;

GENERATE_BODY(BattleArea);
class BattleArea : public Script
{
	FRIEND(BattleArea)
public:
	BattleArea(GameObject* owner);
	~BattleArea();
	void Start() override;
	void Update() override;
	void EnemyDestroyed();
	void ActivateArea(bool activate);;
	void OnCollisionEnter(CollisionData* collisionData);

private:
	int mMaxSimulNumEnemies = 0;
	int mCurrentEnemies = 0;
	int mTotalNumEnemies = 0;
	bool mIsActive = false;
	GameObject* mSpawnerGO1 = nullptr;
	GameObject* mSpawnerGO2 = nullptr;
	GameObject* mSpawnerGO3 = nullptr;
	GameObject* mSpawnerGO4 = nullptr;

	EnemySpawner* mEnemySpawner1 = nullptr;
	EnemySpawner* mEnemySpawner2 = nullptr;
	EnemySpawner* mEnemySpawner3 = nullptr;
	EnemySpawner* mEnemySpawner4 = nullptr;

	BoxColliderComponent* mCollider = nullptr;
};

