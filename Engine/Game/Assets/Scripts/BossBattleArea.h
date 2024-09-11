#pragma once
#include "BattleArea.h"

class EnemyBoss;

GENERATE_BODY(BossBattleArea);
class BossBattleArea : public BattleArea
{
	FRIEND(BossBattleArea)

public:
	BossBattleArea(GameObject* owner) : BattleArea(owner) {}
	~BossBattleArea() {}
	void Start() override;
	void EnemyDestroyed(GameObject* Enemy) override;
	void ActivateArea(bool activate) override;
	void SpawnEnemies() { mNeedsToSpawn = true; }

private:

	GameObject* mBossGO = nullptr;
	EnemyBoss* mBoss = nullptr;
};

#pragma once
