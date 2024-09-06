#pragma once
#include <Script.h>
#include "Enemy.h"
#include "Macros.h"
#include "PoolManager.h"
#include "TimerScript.h"

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
	PoolType GetEnemyType() const { return mPoolType; }
	int GetWaveRounds() const { return mWaveRounds; }
	void Active(bool active) { mIsActive = active; }


private:
	bool mOnlyOnce = false;
	bool mIsActive = false;
	int mSpawnedCounter = 0;
	int mWaveRounds = 1;
	TimerScript mSpawnDelayTimer;
	float mSpawnDelay = 1.25f;

	PoolType mPoolType = PoolType::FIRST;
	PoolManager* mPoolManager = nullptr;

	GameObject* mParticlesGO = nullptr;
	bool mParticlesOn = false;
	TimerScript  mParticlesTimer;
	float mParticlesTime = 0.5f;
};
