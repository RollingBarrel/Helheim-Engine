#pragma once
#include <Script.h>
#include "Enemy.h"
#include "Macros.h"
#include "PoolManager.h"

GENERATE_BODY(Spawner);
class Spawner : public Script
{
	FRIEND(Spawner)
public:
	Spawner(GameObject* owner);

	void Start() override;
	void Update() override;
	bool Spawn();

	bool IsActive() const { return mIsActive; }
	float GetSpawnRate() const { return mSpawnRate; }
	PoolType GetEnemyType() const { return mPoolType; }
	void Active(bool active) { mIsActive = active; }
	void SetSpawnRate(float seconds) { mSpawnRate = seconds; }

private:
	bool mIsActive = false;
	float mSpawnRate = 0.0f;
	float mLastSpawnTime = 0.0f;
	PoolType mPoolType = PoolType::FIRST;

	PoolManager* mPoolManager = nullptr;

};
