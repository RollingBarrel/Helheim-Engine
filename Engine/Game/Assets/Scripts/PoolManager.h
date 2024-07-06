#pragma once
#include "Script.h"
#include "Macros.h"
#include "ObjectPool.h"
#include <map>

enum class PoolType
{
	FIRST = 0,
	
	//ITEMS
	SHIELD,
	BLUE_ENERGY,
	RED_ENERGY,
	BULLET,
	ENEMY_BULLET,

	//ENEMIES
	ROBOT_MELEE,
	ROBOT_RANGE,
	
	LAST
};


GENERATE_BODY(PoolManager);
class PoolManager : public Script
{
	FRIEND(PoolManager);
public:	

	PoolManager(GameObject* owner);

	void Start() override;

	GameObject* Spawn(PoolType type);

private:

	std::map<PoolType, ObjectPool> mPoolMap;
};

