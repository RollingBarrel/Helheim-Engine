#pragma once
#include "Enemy.h"

struct CollisionData;
class BoxColliderComponent;
class BattleArea;
class PoolManager;
class TimerScript;
GENERATE_BODY(EnemyExplosiveSpawner)

class EnemyExplosiveSpawner : public Enemy
{
	FRIEND(EnemyExplosiveSpawner);
	public:
		EnemyExplosiveSpawner(GameObject* owner);

		void Start() override;
		void Update() override;

		bool Spawn();
		bool IsActive() const { return mIsActive; }
		int GetEnemiesPerRound() const { return mEnemiesPerRound; }
		void Active(bool active) { mIsActive = active; }

	private:
		
		bool mOnlyOnce = false;
		bool mIsActive = false;
		int mSpawnedCounter = 0;
		int mEnemiesPerRound = 2;
		float mSpawnRate = 2.0f;

		PoolManager* mPoolManager = nullptr;

		float mGateMoves = 1.0f;
		TimerScript mSpawnTimer;
		TimerScript mOpeningTrap;
		TimerScript mClosingTrap;
		BattleArea* mActiveBattleArea = nullptr;
		bool mIsSpawning = false;
};

