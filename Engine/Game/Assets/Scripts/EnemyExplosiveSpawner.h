#pragma once
#include "Enemy.h"

struct CollisionData;
class BoxColliderComponent;
class BattleArea;
GENERATE_BODY(EnemyExplosiveSpawner)

class EnemyExplosiveSpawner : public Enemy
{
	FRIEND(EnemyExplosiveSpawner);
	public:
		EnemyExplosiveSpawner(GameObject* owner);
		~EnemyExplosiveSpawner() {}

		void Start() override;
		void Update() override;
		void TakeDamage(float damage) override;
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

};

