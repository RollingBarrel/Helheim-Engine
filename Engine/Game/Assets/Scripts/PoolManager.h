#pragma once
#include "Script.h"
#include "Macros.h"
#include "ObjectPool.h"
#include <map>

enum class PoolType
{
	FIRST = 0,
	
	SHIELD,
	BLUE_ENERGY,
	RED_ENERGY,
	BULLET,
	ENEMYBULLET,
	BOMB_TEMPLATE_1,
	BOMB_TEMPLATE_2,
	BOMB_TEMPLATE_3,
	BOMB_TEMPLATE_4,
	
	LAST
};


GENERATE_BODY(PoolManager);
class PoolManager : public Script
{
	FRIEND(PoolManager);
public:	

	PoolManager(GameObject* owner);
	~PoolManager();

	void Start() override;

	GameObject* Spawn(PoolType type);

private:

	std::map<PoolType, ObjectPool> mPoolMap;
	unsigned int mNumItems = 10;
};

