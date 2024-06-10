#pragma once
#include <Script.h>
#include "Macros.h"

GENERATE_BODY(BattleArea);
class BattleArea : public Script
{
	FRIEND(BattleArea)
public:
	BattleArea(GameObject* owner);
	~BattleArea();
	void Start() override;
	void Update() override;

private:
	int mMaxNumEnemies = 0;
	int mCurrentEnemies = 0;
	GameObject* mSpawner1 = nullptr;
	GameObject* mSpawner2 = nullptr;
	GameObject* mSpawner3 = nullptr;
	GameObject* mSpawner4 = nullptr;
};

