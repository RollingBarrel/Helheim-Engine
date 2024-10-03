#include "BossBattleArea.h"
#include "ScriptComponent.h"
#include "GameObject.h"
#include "EnemyBoss.h"
#include "GameManager.h"

CREATE(BossBattleArea)
{
	CLASS(owner);

	MEMBER(MemberType::GAMEOBJECT, mBossGO);
	MEMBER(MemberType::GAMEOBJECT, mSpawnerGO1);
	MEMBER(MemberType::GAMEOBJECT, mSpawnerGO2);
	MEMBER(MemberType::GAMEOBJECT, mSpawnerGO3);
	MEMBER(MemberType::GAMEOBJECT, mSpawnerGO4);
	SEPARATOR("DOORS");
	MEMBER(MemberType::GAMEOBJECT, mAreaDoorsGO);
	SEPARATOR("EXPLOSIVE SPAWNERS");
	MEMBER(MemberType::GAMEOBJECT, mExplosiveSpawn1);
	MEMBER(MemberType::GAMEOBJECT, mExplosiveSpawn2);
	MEMBER(MemberType::GAMEOBJECT, mExplosiveSpawn3);
	MEMBER(MemberType::GAMEOBJECT, mExplosiveSpawn4);
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
	mCurrentEnemies--;
	if (mCurrentEnemies == 0 && mBoss)
	{
		//Reactivate Boss
		mBoss->WakeUp();
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
	if (!mCameraActivated)
	{
		GameManager::GetInstance()->ActivateBossCamera(15.0f);
		mCameraActivated = true;
	}
	BattleArea::ActivateArea(activate);
	mBoss->WakeUp();
	mNeedsToSpawn = false;
}