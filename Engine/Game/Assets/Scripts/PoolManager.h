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
	ROBOT_MELEE,		//6
	ROBOT_RANGE,		//7
	ROBOT_EXPLOSIVE,	//8
	CREATURE_MELEE,		//9
	CREATURE_RANGE,		//10

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

