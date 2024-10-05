#pragma once
#include <Script.h>
#include "Macros.h"
#include "vector"
struct CollisionData;
class Spawner;
class EnemyExplosiveSpawner;
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
	virtual void EnemyDestroyed(GameObject* Enemy);
	virtual void ActivateArea(bool activate);
	void OnCollisionEnter(CollisionData* collisionData);
	bool GetIsAreaActive() { return mHasBeenActivated; }
	int GetCurrentEnemies() const { return mCurrentEnemies; }
	int GetCurrentWave() const { return mWavesRounds; }
	void SetWaves(int waves) { mWavesRounds = waves; }
	void SetSpawnEnemies(bool state) { mSpawnEnemies = state; }

protected:
	void CloseDoors(bool close);

	int mCurrentEnemies = 0;
	int mWavesRounds = 0;
	bool mHasBeenActivated = false;
	bool mIsTutorialArea = false;
	bool mNeedsToSpawn = false;
	bool mSpawnEnemies = true;

	GameObject* mSpawnerGO1 = nullptr;
	GameObject* mSpawnerGO2 = nullptr;
	GameObject* mSpawnerGO3 = nullptr;
	GameObject* mSpawnerGO4 = nullptr;

	Spawner* mEnemySpawner1 = nullptr;
	Spawner* mEnemySpawner2 = nullptr;
	Spawner* mEnemySpawner3 = nullptr;
	Spawner* mEnemySpawner4 = nullptr;
	
	GameObject* mExplosiveSpawn1 = nullptr;
	GameObject* mExplosiveSpawn2 = nullptr;
	GameObject* mExplosiveSpawn3 = nullptr;
	GameObject* mExplosiveSpawn4 = nullptr;

	EnemyExplosiveSpawner* mEnemyExplosiveSpawner1 = nullptr;
	EnemyExplosiveSpawner* mEnemyExplosiveSpawner2 = nullptr;
	EnemyExplosiveSpawner* mEnemyExplosiveSpawner3 = nullptr;
	EnemyExplosiveSpawner* mEnemyExplosiveSpawner4= nullptr;

	GameObject* mAreaDoorsGO = nullptr;
	
	std::vector<Spawner*> mSpawners;
	std::vector<EnemyExplosiveSpawner*> mExplosiveSpawners;
	GameObject* mElevator = nullptr;
	BoxColliderComponent* mCollider = nullptr;
};