#pragma once
#include "Enemy.h"
class GameObject;
class ExplosiveTrap;

GENERATE_BODY(EnemyExplosiveTrap);
class EnemyExplosiveTrap : public Enemy
{
	FRIEND(EnemyExplosiveTrap)
public:
	EnemyExplosiveTrap(GameObject* owner) : Enemy(owner) {}
	~EnemyExplosiveTrap() {}

	void Start() override;
	void Update() override;
	void TakeDamage(float damage) override;

private:
	GameObject* mOuterTrap = nullptr;
	ExplosiveTrap* mTrap = nullptr;
};