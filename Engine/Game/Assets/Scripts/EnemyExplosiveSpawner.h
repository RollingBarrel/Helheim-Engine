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
		void Idle();

	private:
		enum class EnemyState {
			IDLE,
			SPAWNING,
			DEATH
		};
		 
		void Spawning();
		void Die();
		void OnCollisionEnter(CollisionData* collisionData);

		EnemyState mCurrentState = EnemyState::IDLE;
		BoxColliderComponent* mCollider = nullptr;
		PoolManager* mPoolManager = nullptr;

		float mSpawnRate = 2.0f;
		float mLastSpawnTime = 0.0f;
		int mMaxActiveEnemies = 12;
		int* mCurrentAreaEnemies = nullptr;
		TimerScript mAnimationTimer;
		SpawnState mTrapState = SpawnState::IDLE;
};

