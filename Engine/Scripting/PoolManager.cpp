#include "PoolManager.h"
#include "GameObject.h"

CREATE(PoolManager)
{
	CLASS(owner);
	END_CREATE;
}

PoolManager::PoolManager(GameObject* owner) : Script(owner)
{
}


void PoolManager::Start()
{

	unsigned int numBullets = 50;
	unsigned int numItems = 10;
	unsigned int numEnemies = 15;


	//Items
	GameObject* Shield = new GameObject("Shield_Pool", mGameObject);
	GameObject* BlueEnergy = new GameObject("BlueEnergy_Pool", mGameObject);
	GameObject* RedEnergy = new GameObject("RedEnergy_Pool", mGameObject);
	GameObject* Bullet = new GameObject("Bullet_Pool", mGameObject);
	GameObject* EnemyBullet = new GameObject("EnemyBullet_Pool", mGameObject);

	//Enemies
	GameObject* EnemyRobotMelee = new GameObject("EnemyRobotMelee_Pool", mGameObject);
	GameObject* EnemyRobotRange = new GameObject("EnemyRobotRange_Pool", mGameObject);
	GameObject* EnemyCreatureMelee = new GameObject("EnemyCreatureMelee_Pool", mGameObject);
	GameObject* EnemyCreatureRange = new GameObject("EnemyCreatureRange_Pool", mGameObject);
	GameObject* EnemyExplosive = new GameObject("EnemyExplosive_Pool", mGameObject);



	mPoolMap =
	{
		//ITEMS
		{PoolType::SHIELD, ObjectPool("Item_Shield.prfb", numItems, Shield)},
		{PoolType::BLUE_ENERGY, ObjectPool("Item_Blue_Battery.prfb", numItems, BlueEnergy)},
		{PoolType::RED_ENERGY, ObjectPool("Item_Red_Battery.prfb", numItems, RedEnergy)},
		{PoolType::BULLET, ObjectPool("Bullet.prfb", numBullets, Bullet)},
		{PoolType::ENEMY_BULLET, ObjectPool("EnemyBullet.prfb", numBullets, EnemyBullet)},

		//ENEMIES
		{ PoolType::ROBOT_MELEE, ObjectPool("Robot_Melee.prfb", numEnemies, EnemyRobotMelee) },
		{ PoolType::ROBOT_RANGE, ObjectPool("Robot_Range.prfb", numEnemies, EnemyRobotRange) },
		{ PoolType::CREATURE_RANGE, ObjectPool("Creature_Range.prfb", numEnemies, EnemyCreatureRange) },
		{ PoolType::CREATURE_MELEE, ObjectPool("Creature_Melee.prfb", numEnemies, EnemyCreatureMelee) },
		{ PoolType::ROBOT_EXPLOSIVE, ObjectPool("Enemy_Explosive.prfb", numEnemies, EnemyExplosive) }

	};

}

GameObject* PoolManager::Spawn(PoolType type)
{
	if (mPoolMap.find(type) != mPoolMap.end())
	{
		return mPoolMap.at(type).GetObject();
	}
	return nullptr;
}
