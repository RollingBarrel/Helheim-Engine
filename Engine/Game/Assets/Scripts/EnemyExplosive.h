#pragma once
#include "Enemy.h"

struct CollisionData;
class BoxColliderComponent;
class GameObject;
GENERATE_BODY(EnemyExplosive);

enum class ExplosiveEnemyState
{
	IDLE,
	CHASE,
	CHARGING,
	EXPLOSION,
	DEATH
};

class EnemyExplosive : public Enemy
{
	FRIEND(EnemyExplosive)
public:
	EnemyExplosive(GameObject* owner);
	~EnemyExplosive() {}
	void Start() override;
	void Update() override;
	void Idle();
	void Chase();

private:
	void Charging();
	void Explosion();
	void ChargeWarningArea();
	void Death() override;

	ExplosiveEnemyState mCurrentState = ExplosiveEnemyState::IDLE;

	float mChargingDistance = 1.0f;
	float mExplosionDistance = 5.0f;
	float mExplosionDamage = 10.0f;
	GameObject* mExplosionWarningGO = nullptr;
	float mWarningInitialSize = 0.1f;
	float mWarningTimer = 0.0f;
	float mExplosionDelay = 2.0f;
	float3 mWarningSize = float3(0.1f, 0.1f, 0.1f);
};