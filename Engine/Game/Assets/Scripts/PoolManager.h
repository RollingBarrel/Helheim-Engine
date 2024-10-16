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
	CREATURE_MELEE,		//8
	CREATURE_RANGE,		//9
	ROBOT_EXPLOSIVE,	//10

	BOSS_BULLET,		//11

	LAST
};

GENERATE_BODY(PoolManager);
class PoolManager : public Script
{
	FRIEND(PoolManager);
public:	

	PoolManager(GameObject* owner) : Script(owner) {}
	~PoolManager() {}

	void Start() override;

	GameObject* Spawn(PoolType type);

private:

	std::map<PoolType, ObjectPool> mPoolMap;
};

