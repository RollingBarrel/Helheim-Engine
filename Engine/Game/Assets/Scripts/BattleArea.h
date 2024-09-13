#pragma once
#include <Script.h>
#include "Macros.h"
#include "vector"
struct CollisionData;
class Spawner;
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
	void EnemyDestroyed(GameObject* Enemy);
	void ActivateArea(bool activate);
	void OnCollisionEnter(CollisionData* collisionData);
	int GetCurrentEnemies() { return mCurrentEnemies; }
	int GetCurrentWave() { return mWavesRounds; }
	int GetCurrentExplosiveEnemies() { return mCurrentExplosiveEnemies; }
	void SetWaves(int waves) { mWavesRounds = waves; }; 
	void AddExplosiveEnemy();

private:

	void SetTrapState(GameObject* trap, bool enable);
	void UpdateTrapNumber();

	int mCurrentEnemies = 0;
	int mWavesRounds = 0;
	int mCurrentExplosiveEnemies = 0;
	int mCurrentTraps = 0;
	bool mHasBeenActivated = false;
	bool mIsTutorialArea = false;

	bool mNeedsToSpawn = false;
	GameObject* mSpawnerGO1 = nullptr;
	GameObject* mSpawnerGO2 = nullptr;
	GameObject* mSpawnerGO3 = nullptr;
	GameObject* mSpawnerGO4 = nullptr;

	Spawner* mEnemySpawner1 = nullptr;
	Spawner* mEnemySpawner2 = nullptr;
	Spawner* mEnemySpawner3 = nullptr;
	Spawner* mEnemySpawner4 = nullptr;
	
	GameObject* mTrap1 = nullptr;
	GameObject* mTrap2 = nullptr;
	GameObject* mTrap3 = nullptr;
	GameObject* mTrap4 = nullptr;

	GameObject* mAreaDoorsGO = nullptr;
	

	std::vector<Spawner*> mSpawners;
	GameObject* mElevator = nullptr;
	BoxColliderComponent* mCollider = nullptr;
};

