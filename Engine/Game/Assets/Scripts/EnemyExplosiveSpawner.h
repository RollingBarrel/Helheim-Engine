#pragma once
#include "Enemy.h"

struct CollisionData;
class BoxColliderComponent;
class BattleArea;
class PoolManager;
class TimerScript;
GENERATE_BODY(EnemyExplosiveSpawner)

enum SpawnState
{
	IDLE,
	OPENING,
	CLOSING
};

class EnemyExplosiveSpawner : public Enemy
{
	FRIEND(EnemyExplosiveSpawner);
	public:
		EnemyExplosiveSpawner(GameObject* owner);
		~EnemyExplosiveSpawner() {}

		void Start() override;
		void Update() override;
		void TakeDamage(float damage) override;
		void PushBack() override;
		void Death() override;

	private:
		enum class EnemyState {
			IDLE,
			SPAWNING,
			DEATH
		};
		 
		void Idle();
		void Spawning();

		EnemyState mCurrentState = EnemyState::IDLE;
		PoolManager* mPoolManager = nullptr;

		float mSpawnRate = 2.0f;
		TimerScript mSpawnTimer;
		int mMaxActiveEnemies = 12;
		BattleArea* mActiveBattleArea = nullptr;
		TimerScript mAnimationTimer;
		SpawnState mTrapState = SpawnState::IDLE;
};

