#pragma once
#include <Enemy.h>

GENERATE_BODY(EnemyExplosiveSpawner)
class EnemyExplosiveSpawner : public Enemy
{
	FRIEND(EnemyExplosiveSpawner);
	public:
		EnemyExplosiveSpawner(GameObject* owner);
		~EnemyExplosiveSpawner() {}

	private:

};

