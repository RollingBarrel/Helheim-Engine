#include "BattleArea.h"
#include "ScriptComponent.h"
#include "BoxColliderComponent.h"
#include "GameObject.h"

#include "GameManager.h"
#include "HudController.h"
#include "Spawner.h"

CREATE(BattleArea)
{
	CLASS(owner);

	MEMBER(MemberType::GAMEOBJECT, mSpawnerGO1);
	MEMBER(MemberType::GAMEOBJECT, mSpawnerGO2);
	MEMBER(MemberType::GAMEOBJECT, mSpawnerGO3);
	MEMBER(MemberType::GAMEOBJECT, mSpawnerGO4);
	MEMBER(MemberType::INT, mMaxSimulNumEnemies);
	MEMBER(MemberType::INT, mTotalNumEnemies);
	END_CREATE;
}

BattleArea::BattleArea(GameObject* owner) : Script(owner)
{
}

BattleArea::~BattleArea()
{
}

void BattleArea::Start()
{
	if (mSpawnerGO1)
	{
		mEnemySpawner1 = reinterpret_cast<Spawner*>(reinterpret_cast<ScriptComponent*>(mSpawnerGO1->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
	}
	if (mSpawnerGO2)
	{
		mEnemySpawner2 = reinterpret_cast<Spawner*>(reinterpret_cast<ScriptComponent*>(mSpawnerGO2->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
	}
	if (mSpawnerGO3)
	{
		mEnemySpawner3 = reinterpret_cast<Spawner*>(reinterpret_cast<ScriptComponent*>(mSpawnerGO3->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
	}
	if (mSpawnerGO4)
	{
		mEnemySpawner4 = reinterpret_cast<Spawner*>(reinterpret_cast<ScriptComponent*>(mSpawnerGO4->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
	}

	mCollider = reinterpret_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
	if (mCollider)
	{
		mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&BattleArea::OnCollisionEnter, this, std::placeholders::_1)));
	}

}

void BattleArea::Update()
{
	if (mIsActive && mTotalNumEnemies > 0)
	{
		if (mEnemySpawner1 && mCurrentEnemies < mMaxSimulNumEnemies)
		{
			if (mEnemySpawner1->Spawn())
			{
				mCurrentEnemies++;
				mTotalNumEnemies--;
			}
		}
		if (mEnemySpawner2 && mCurrentEnemies < mMaxSimulNumEnemies)
		{
			if (mEnemySpawner2->Spawn())
			{
				mCurrentEnemies++;
			}
		}
		if (mEnemySpawner3 && mCurrentEnemies < mMaxSimulNumEnemies)
		{
			if (mEnemySpawner3->Spawn())
			{
				mCurrentEnemies++;
			}
		}
		if (mEnemySpawner4 && mCurrentEnemies < mMaxSimulNumEnemies)
		{
			if (mEnemySpawner4->Spawn())
			{
				mCurrentEnemies++;
			}
		}
	}
}

void BattleArea::EnemyDestroyed()
{
	mCurrentEnemies--;
	if (mTotalNumEnemies <= 0 && mCurrentEnemies <= 0)
	{
		ActivateArea(false);
		return;
	}
	//LOG("REMAINING ENEMIES: %i", mTotalNumEnemies);
}

inline void BattleArea::ActivateArea(bool activate)
{
	mIsActive = activate;
	if (mEnemySpawner1)
	{
		mEnemySpawner1->Active(activate);
	}
	if (mEnemySpawner2)
	{
		mEnemySpawner2->Active(activate);
	}
	if (mEnemySpawner3)
	{
		mEnemySpawner3->Active(activate);
	}
	if (mEnemySpawner4)
	{
		mEnemySpawner4->Active(activate);
	}

	if (!activate)
	{
		GameManager::GetInstance()->SetActiveBattleArea(nullptr);
		GameManager::GetInstance()->GetHud()->SetSanity();
		//LOG("Sanity");
	}
}


void BattleArea::OnCollisionEnter(CollisionData* collisionData)
{
	if (collisionData->collidedWith->GetTag().compare("Player") == 0 && !mIsActive)
	{
		GameManager::GetInstance()->SetActiveBattleArea(this);
		ActivateArea(true);
		//LOG("PLAYER COLLISION");
	}
}