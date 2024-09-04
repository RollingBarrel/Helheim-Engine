#pragma once
#include "Enemy.h"
class GameObject;

GENERATE_BODY(EnemyExplosiveTrap);
class EnemyExplosiveTrap : public Enemy
{
	FRIEND(EnemyExplosiveTrap)
public:
	EnemyExplosiveTrap(GameObject* owner) : Enemy(owner) {}
	~EnemyExplosiveTrap() {}

	void Update() override;
	void TakeDamage(float damage) override;

private:
	GameObject* mOuterTrap = nullptr;
	bool mIsActive = false;
};