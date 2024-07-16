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

PoolManager::~PoolManager()
{
}

void PoolManager::Start()
{
	GameObject* Shield = new GameObject("Shield_Pool", mGameObject);
	GameObject* BlueEnergy = new GameObject("BlueEnergy_Pool", mGameObject);
	GameObject* RedEnergy = new GameObject("RedEnergy_Pool", mGameObject);
	GameObject* Bullet = new GameObject("Bullet_Pool", mGameObject);
	GameObject* EnemyBullet = new GameObject("EnemyBullet_Pool", mGameObject);
	GameObject* EnemyBombs = new GameObject("EnemyBombs_Pool", mGameObject);

	unsigned int numBullets = 50;

	mPoolMap =
	{
		{PoolType::SHIELD, ObjectPool("Item_Shield.prfb", mNumItems, Shield)},
		{PoolType::BLUE_ENERGY, ObjectPool("Item_BlueEnergy.prfb", mNumItems, BlueEnergy)},
		{PoolType::RED_ENERGY, ObjectPool("Item_RedEnergy.prfb", mNumItems, RedEnergy)},
		{PoolType::BULLET, ObjectPool("Bullet.prfb", numBullets, Bullet)},
		{PoolType::ENEMYBULLET, ObjectPool("EnemyBullet.prfb", numBullets, EnemyBullet)},
		{PoolType::BOMB_TEMPLATE_1, ObjectPool("BombingTemplate1.prfb", 1, EnemyBombs)},
		{PoolType::BOMB_TEMPLATE_2, ObjectPool("BombingTemplate2.prfb", 1, EnemyBombs)},
		{PoolType::BOMB_TEMPLATE_3, ObjectPool("BombingTemplate3.prfb", 1, EnemyBombs)},
		{PoolType::BOMB_TEMPLATE_4, ObjectPool("BombingTemplate4.prfb", 1, EnemyBombs)}
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
