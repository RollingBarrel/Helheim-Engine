#pragma once
#include <Script.h>
#include "Enemy.h"
#include "Macros.h"

GENERATE_BODY(EnemySpawner);
class EnemySpawner : public Script
{
	FRIEND(EnemySpawner)
public:
	EnemySpawner(GameObject* owner);

	void Start() override;
	void Update() override;
	void Spawn();

	bool GetIsActive() { return mIsActive; }
	float GetSpawnRate() { return mSpawnRate; }
	EnemyType GetEnemyType() { return mEnemyType; }
	void SetIsActive(bool active) { mIsActive = active; }
	void SetSpawnRate(float seconds) { mSpawnRate = seconds; }
	void SetEnemyType(EnemyType enemyType) { mEnemyType = enemyType; }

private:
	bool mIsActive = false;
	float mSpawnRate = 0.0f;
	float mLastSpawnTime = 0.0f;
	EnemyType mEnemyType = EnemyType::ROBOT_MELEE;

};
