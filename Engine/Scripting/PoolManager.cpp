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
	GameObject* Shield = new GameObject("Shield", mGameObject);
	GameObject* BlueEnergy = new GameObject("BlueEnergy", mGameObject);
	GameObject* RedEnergy = new GameObject("BlueEnergy", mGameObject);

	mPoolMap =
	{
		{PoolType::SHIELD, ObjectPool("Item_Shield.prfb", mNumItems, Shield)},
		{PoolType::BLUE_ENERGY, ObjectPool("Item_BlueEnergy.prfb", mNumItems, BlueEnergy)},
		{PoolType::RED_ENERGY, ObjectPool("Item_RedEnergy.prfb", mNumItems, RedEnergy)},
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
