#pragma once
#include "Enemy.h"

GENERATE_BODY(EnemyCreatureMelee);
class EnemyCreatureMelee : public Enemy
{
	FRIEND(EnemyCreatureMelee)

public:
	EnemyCreatureMelee(GameObject* owner) : Enemy(owner) {}
	~EnemyCreatureMelee() {}

private:
	void Attack() override;
	void Death() override;

	float mMeeleDamage = 10.0f;
};

