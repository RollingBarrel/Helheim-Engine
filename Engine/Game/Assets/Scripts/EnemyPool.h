#pragma once
#include <Script.h>
#include "Macros.h"
#include "Enemy.h"

#include <vector>

GENERATE_BODY(EnemyPool);
class EnemyPool : public Script
{
	FRIEND(EnemyPool)
public:
	EnemyPool(GameObject* owner);
	~EnemyPool();
	void Start() override;
	void Update() override;

	GameObject* GetEnemy(EnemyType type) const;

private:
	const char* GetPrefabFromEnemyType(EnemyType enemyType);
	
	std::vector<GameObject*> mEnemies[static_cast<int>(EnemyType::COUNT)];
	int mNumOfEnemies = 10;

};
