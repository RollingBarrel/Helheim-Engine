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
	void ActivateArea() { mIsActive = true; };
	void DestroyEnemy();

private:
	int mMaxSimulNumEnemies = 0;
	int mCurrentEnemies = 0;
	int mTotalNumEnemies = 0;
	bool mIsActive = false;
	GameObject* mSpawner1 = nullptr;
	GameObject* mSpawner2 = nullptr;
	GameObject* mSpawner3 = nullptr;
	GameObject* mSpawner4 = nullptr;
};

