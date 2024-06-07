#pragma once
#include <Script.h>
#include "Macros.h"
#include "Enemy.h"

#include <vector>

GENERATE_BODY(EnemyPool);
class EnemyPool : public Script
{
public:
	EnemyPool(GameObject* owner);
	~EnemyPool();
	void Start() override;
	void Update() override;

	GameObject* GetEnemy(EnemyType type);

private:
	std::vector<GameObject*> mEnemies[static_cast<int>(EnemyType::COUNT)];
	int mPoolCount = 0;

};
