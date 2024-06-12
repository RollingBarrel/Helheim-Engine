#pragma once
#include <Script.h>
#include "Enemy.h"
#include "Macros.h"

GENERATE_BODY(EnemySpawner);
class EnemySpawner : public Script
{
public:
	EnemySpawner(GameObject* owner);

	void Start() override;
	void Update() override;
	void Spawn();

	bool GetIsActive() { return mIsActive; }
	float GetSpawnRate() { return mSpawnRate; }
	int GetMaxEnemies() { return mMaxEnemies; }
	void SetIsActive(bool active) { mIsActive = active; }
	void SetSpawnRate(float seconds) { mSpawnRate = seconds; }
	void SetMaxEnemies(int maxEnemies) { mMaxEnemies = maxEnemies; }

private:
	bool mIsActive = false;
	float mSpawnRate = 0.0f;
	float mLastSpawnTime = 0.0f;
	int mMaxEnemies = 0;
	EnemyType mEnemyType = EnemyType::ROBOT_MELEE;

};
