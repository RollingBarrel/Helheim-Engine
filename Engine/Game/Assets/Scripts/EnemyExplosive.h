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

	void Start() override;
	void Charge() override;
	void Attack() override;

private:
	void ChargeWarningArea();

	float mExplosionRadius = 5.0f;
	GameObject* mExplosionWarningGO = nullptr;
	GameObject* mExplosionParticle = nullptr;
	float3 mWarningSize = float3(0.1f, 0.1f, 0.1f);
	PlayerController* playerScript = nullptr;

};