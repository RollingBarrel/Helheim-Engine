#pragma once
#include <Script.h>
#include "Enemy.h"
#include "Macros.h"

class EnemyPool;

GENERATE_BODY(EnemySpawner);
class EnemySpawner : public Script
{
	FRIEND(EnemySpawner)
public:
	EnemySpawner(GameObject* owner);

	void Start() override;
	void Update() override;
	bool Spawn();

	bool IsActive() const { return mIsActive; }
	float GetSpawnRate() const { return mSpawnRate; }
	EnemyType GetEnemyType() const { return mEnemyType; }
	void Active(bool active) { mIsActive = active; }
	void SetSpawnRate(float seconds) { mSpawnRate = seconds; }
	void SetEnemyType(EnemyType enemyType) { mEnemyType = enemyType; }

private:
	bool mIsActive = false;
	float mSpawnRate = 0.0f;
	float mLastSpawnTime = 0.0f;
	EnemyType mEnemyType = EnemyType::ROBOT_MELEE;

	EnemyPool* mEnemyPool = nullptr;

};
