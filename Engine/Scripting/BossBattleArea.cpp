#include "BossBattleArea.h"
#include "ScriptComponent.h"
#include "GameObject.h"
#include "EnemyBoss.h"

CREATE(BossBattleArea)
{
	CLASS(owner);

	MEMBER(MemberType::GAMEOBJECT, mBossGO);
	MEMBER(MemberType::GAMEOBJECT, mSpawnerGO1);
	MEMBER(MemberType::GAMEOBJECT, mSpawnerGO2);
	MEMBER(MemberType::GAMEOBJECT, mSpawnerGO3);
	MEMBER(MemberType::GAMEOBJECT, mSpawnerGO4);
	SEPARATOR("DOORS");
	MEMBER(MemberType::GAMEOBJECT, mDoor1);
	MEMBER(MemberType::GAMEOBJECT, mDoor2);
	MEMBER(MemberType::GAMEOBJECT, mElevator);
	SEPARATOR("TRAPS");
	MEMBER(MemberType::GAMEOBJECT, mTrap1);
	MEMBER(MemberType::GAMEOBJECT, mTrap2);
	MEMBER(MemberType::GAMEOBJECT, mTrap3);
	MEMBER(MemberType::GAMEOBJECT, mTrap4);
	END_CREATE;
}

void BossBattleArea::Start()
{
	BattleArea::Start();
	if (mBossGO)
	{
		mBoss = static_cast<EnemyBoss*>(static_cast<ScriptComponent*>(mBossGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
	}
}

void BossBattleArea::EnemyDestroyed(GameObject* enemy)
{
	std::string scriptName = reinterpret_cast<ScriptComponent*>(enemy->GetComponent(ComponentType::SCRIPT))->GetScriptName();

	if (scriptName == "EnemyExplosiveSpawner")
	{
		mCurrentTraps--;
	}
	else // Any enemy except traps and explosive enemies
	{
		mCurrentEnemies--;
		if (mCurrentEnemies == 0 && mBoss)
		{
			//Reactivate Boss
			mBoss->WakeUp();
		}

	}

	/*if (mWavesRounds <= 0 && mCurrentExplosiveEnemies <= 0 && mCurrentTraps <= 0)
	{

		ActivateArea(false);
		mGameObject->SetEnabled(false);
		return;
	}*/
	//LOG("REMAINING ENEMIES: %i", mTotalNumEnemies);
}

inline void BossBattleArea::ActivateArea(bool activate)
{
	BattleArea::ActivateArea(activate);
	mNeedsToSpawn = false;
}