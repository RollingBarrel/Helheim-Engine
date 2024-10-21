#pragma once
#include "Script.h"
#include "Macros.h"
#include "ObjectPool.h"
#include <map>

enum class PoolType
{
	FIRST = 0,
	
	//ITEMS
	SHIELD = 1,
	BLUE_ENERGY = 2,
	RED_ENERGY = 3,
	BULLET = 4,
	ENEMY_BULLET = 5,
	BOSS_BULLET = 11,
	RED_BULLET = 12,
	BLUE_BULLET = 13,

	//ENEMIES
	ROBOT_MELEE = 6,
	ROBOT_RANGE = 7,
	CREATURE_MELEE = 8,
	CREATURE_RANGE = 9,
	ROBOT_EXPLOSIVE = 10,


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

