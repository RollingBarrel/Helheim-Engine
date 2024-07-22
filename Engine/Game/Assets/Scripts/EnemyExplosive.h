#pragma once
#include "Enemy.h"


struct CollisionData;
class BoxColliderComponent;
class GameObject;
GENERATE_BODY(EnemyExplosive);
class EnemyExplosive : public Enemy
{
	FRIEND(EnemyExplosive)
	public:
		EnemyExplosive(GameObject* owner);
		~EnemyExplosive() {}

		void Start() override;
		void Update() override;
		void Init() override;

		void Idle();
		void Chase();
		void TakeDamage(float damage) override;

	private:
		enum class EnemyState {
			IDLE,
			CHASE,
			CHARGING,
			EXPLOSION,
			DEATH
		};

		void Charging();
		void Explosion();
		void ChargeWarningArea();
		void Die();
		void OnCollisionEnter(CollisionData* collisionData);

		EnemyState mCurrentState = EnemyState::IDLE;

		float mChargingDistance = 1.0f;
		float mExplosionDistance = 5.0f;
		float mExplosionDamage = 10.0f;
		GameObject* mExplosionWarningGO = nullptr;
		float mWarningInitialSize = 0.1f;
		float mWarningTimer = 0.0f;
		float mExplosionDelay = 2.0f;
		float3 mWarningSize = float3(0.1f,0.1f,0.1f);
		//Collider
		BoxColliderComponent* mCollider = nullptr;
};