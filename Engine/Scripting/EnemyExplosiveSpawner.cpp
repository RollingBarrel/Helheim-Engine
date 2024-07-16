#include "EnemyExplosiveSpawner.h"
#include "GameObject.h"

CREATE(EnemyExplosiveSpawner)
{
    CLASS(owner);
    SEPARATOR("STATS");
    END_CREATE;
}

EnemyExplosiveSpawner::EnemyExplosiveSpawner(GameObject* owner) : Enemy(owner)
{
	//TODO: Set the enemy values
}

