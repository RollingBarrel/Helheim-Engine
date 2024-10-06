#pragma once
#include "Enemy.h"

class GameObject;
class ParticleSystemComponent;
class PlayerController;

GENERATE_BODY(EnemyExplosive);
class EnemyExplosive : public Enemy
{
	FRIEND(EnemyExplosive)
public:
	EnemyExplosive(GameObject* owner) : Enemy(owner) {}
	~EnemyExplosive() {}

	void Update() override;
	void Start() override;
	void Charge() override;
	void Attack() override;
	void Death() override;
	void TakeDamage(float damage) override;

private:
	void ChargeWarningArea();

	float mExplosionRadius = 5.0f;
	GameObject* mExplosionWarningGO = nullptr;
	GameObject* mExplosionParticle = nullptr;
	PlayerController* playerScript = nullptr;

	int mChargeSound = -1;
	int mAttackSound = -1;
	int mStepSound = -1;

	bool mChargePlaying = false;
	bool mExplosionPlaying = false;

};