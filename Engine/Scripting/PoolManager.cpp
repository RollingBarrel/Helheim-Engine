#include "PoolManager.h"
#include "Application.h"
#include "ModuleScene.h"

#include "GameObject.h"

CREATE(PoolManager)
{
	CLASS(owner);
	END_CREATE;
}


void PoolManager::Start()
{

	unsigned int numBullets = 150;
	unsigned int numItems = 15;
	unsigned int numEnemies = 7;


	//Items
	GameObject* Shield = new GameObject("Shield_Pool", mGameObject);
	GameObject* BlueEnergy = new GameObject("BlueEnergy_Pool", mGameObject);
	GameObject* RedEnergy = new GameObject("RedEnergy_Pool", mGameObject);
	GameObject* Bullet = new GameObject("Bullet_Pool", mGameObject);
	GameObject* EnemyBullet = new GameObject("EnemyBullet_Pool", mGameObject);
	GameObject* BossBullet = new GameObject("BossBullet_Pool", mGameObject);

	//Enemies
	GameObject* EnemyRobotMelee = new GameObject("EnemyRobotMelee_Pool", mGameObject);
	GameObject* EnemyRobotRange = new GameObject("EnemyRobotRange_Pool", mGameObject);
	GameObject* EnemyCreatureMelee = new GameObject("EnemyCreatureMelee_Pool", mGameObject);
	GameObject* EnemyCreatureRange = new GameObject("EnemyCreatureRange_Pool", mGameObject);
	GameObject* EnemyExplosive = new GameObject("EnemyExplosive_Pool", mGameObject);

	mPoolMap.insert({ PoolType::SHIELD, ObjectPool("Item_Shield.prfb", numItems, Shield) });
	mPoolMap.insert({ PoolType::BLUE_ENERGY, ObjectPool("Item_Blue_Battery.prfb", numItems, BlueEnergy) });
	mPoolMap.insert({ PoolType::RED_ENERGY, ObjectPool("Item_Red_Battery.prfb", numItems, RedEnergy) });
	mPoolMap.insert({ PoolType::BULLET, ObjectPool("Bullet.prfb", numBullets, Bullet) });


	if (App->GetScene()->GetName().compare("Level1Scene") == 0)
	{
		mPoolMap.insert({ PoolType::ENEMY_BULLET, ObjectPool("EnemyBullet.prfb", numBullets, EnemyBullet) });
		mPoolMap.insert({ PoolType::ROBOT_MELEE, ObjectPool("Robot_Melee.prfb", numEnemies, EnemyRobotMelee) });
		mPoolMap.insert({ PoolType::ROBOT_RANGE, ObjectPool("Robot_Range.prfb", numEnemies, EnemyRobotRange) });
		mPoolMap.insert({ PoolType::ROBOT_EXPLOSIVE, ObjectPool("Enemy_Explosive.prfb", numEnemies, EnemyExplosive) });
		mPoolMap.insert({ PoolType::CREATURE_MELEE, ObjectPool("Creature_Melee.prfb", 3, EnemyCreatureMelee) });
	}
	else if (App->GetScene()->GetName().compare("Level2Scene") == 0)
	{
		mPoolMap.insert({ PoolType::CREATURE_MELEE, ObjectPool("Creature_Melee.prfb", numEnemies, EnemyCreatureMelee) });
		mPoolMap.insert({ PoolType::CREATURE_RANGE, ObjectPool("Creature_Range.prfb", numEnemies, EnemyCreatureRange) });
		mPoolMap.insert({ PoolType::ROBOT_MELEE, ObjectPool("Robot_Melee.prfb", 4, EnemyRobotMelee) });
	}
	else if (App->GetScene()->GetName().compare("Level3Scene") == 0)
	{
		mPoolMap.insert({ PoolType::BOSS_BULLET, ObjectPool("BossBullet.prfb", numBullets, BossBullet) });

		mPoolMap.insert({ PoolType::CREATURE_MELEE, ObjectPool("Creature_Melee.prfb", numEnemies, EnemyCreatureMelee) });
		mPoolMap.insert({ PoolType::CREATURE_RANGE, ObjectPool("Creature_Range.prfb", numEnemies, EnemyCreatureRange) });
	}
	//mPoolMap =
	//{
	//	//ITEMS
	//	{PoolType::SHIELD, ObjectPool("Item_Shield.prfb", numItems, Shield)},
	//	{PoolType::BLUE_ENERGY, ObjectPool("Item_Blue_Battery.prfb", numItems, BlueEnergy)},
	//	{PoolType::RED_ENERGY, ObjectPool("Item_Red_Battery.prfb", numItems, RedEnergy)},
	//	{PoolType::BULLET, ObjectPool("Bullet.prfb", numBullets, Bullet)},
	//	{PoolType::ENEMY_BULLET, ObjectPool("EnemyBullet.prfb", numBullets, EnemyBullet)},
	//	{PoolType::BOSS_BULLET, ObjectPool("BossBullet.prfb", numBullets, BossBullet)},
	//
	//	//ENEMIES
	//	{ PoolType::ROBOT_MELEE, ObjectPool("Robot_Melee.prfb", numEnemies, EnemyRobotMelee) },
	//	{ PoolType::ROBOT_RANGE, ObjectPool("Robot_Range.prfb", numEnemies, EnemyRobotRange) },
	//	{ PoolType::CREATURE_RANGE, ObjectPool("Creature_Range.prfb", numEnemies, EnemyCreatureRange) },
	//	{ PoolType::CREATURE_MELEE, ObjectPool("Creature_Melee.prfb", numEnemies, EnemyCreatureMelee) },
	//	{ PoolType::ROBOT_EXPLOSIVE, ObjectPool("Enemy_Explosive.prfb", numEnemies, EnemyExplosive) }
	//
	//};

}

GameObject* PoolManager::Spawn(PoolType type)
{
	if (mPoolMap.find(type) != mPoolMap.end())
	{
		return mPoolMap.at(type).GetObject();
	}
	return nullptr;
}
