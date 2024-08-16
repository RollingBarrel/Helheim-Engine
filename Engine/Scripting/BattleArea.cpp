#include "BattleArea.h"
#include "ScriptComponent.h"
#include "BoxColliderComponent.h"
#include "AnimationComponent.h"
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
	SEPARATOR("DOORS");
	MEMBER(MemberType::GAMEOBJECT, mDoor1);
	MEMBER(MemberType::GAMEOBJECT, mDoor2);
	SEPARATOR("TRAPS");
	MEMBER(MemberType::GAMEOBJECT, mTrap1);
	MEMBER(MemberType::GAMEOBJECT, mTrap2);
	MEMBER(MemberType::GAMEOBJECT, mTrap3);
	MEMBER(MemberType::GAMEOBJECT, mTrap4);
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
		mEnemySpawner1 = static_cast<Spawner*>(static_cast<ScriptComponent*>(mSpawnerGO1->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
	}
	if (mSpawnerGO2)
	{
		mEnemySpawner2 = static_cast<Spawner*>(static_cast<ScriptComponent*>(mSpawnerGO2->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
	}
	if (mSpawnerGO3)
	{
		mEnemySpawner3 = static_cast<Spawner*>(static_cast<ScriptComponent*>(mSpawnerGO3->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
	}
	if (mSpawnerGO4)
	{
		mEnemySpawner4 = static_cast<Spawner*>(static_cast<ScriptComponent*>(mSpawnerGO4->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
	}

	mCollider = static_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
	if (mCollider)
	{
		mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&BattleArea::OnCollisionEnter, this, std::placeholders::_1)));
	}

	UpdateTrapNumber();

	CloseDoors(false);
}

void BattleArea::Update()
{
	if (mHasBeenActivated && mTotalNumEnemies > 0)
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
				mTotalNumEnemies--;
			}
		}
		if (mEnemySpawner3 && mCurrentEnemies < mMaxSimulNumEnemies)
		{
			if (mEnemySpawner3->Spawn())
			{
				mCurrentEnemies++;
				mTotalNumEnemies--;
			}
		}
		if (mEnemySpawner4 && mCurrentEnemies < mMaxSimulNumEnemies)
		{
			if (mEnemySpawner4->Spawn())
			{
				mCurrentEnemies++;
				mTotalNumEnemies--;
			}
		}
	}
}

void BattleArea::EnemyDestroyed(GameObject* enemy)
{
	std::string scriptName = reinterpret_cast<ScriptComponent*>(enemy->GetComponent(ComponentType::SCRIPT))->GetScriptName();

	if (scriptName == "EnemyExplosiveSpawner")
	{
		mCurrentTraps--;
	}
	else if (scriptName == "EnemyExplosive")
	{
		mCurrentExplosiveEnemies--;
	}
	else // Any enemy except traps and explosive enemies
	{
		mCurrentEnemies--;
	}

	LOG("CURRENT TRAPS: %i", mCurrentTraps);
	LOG("CURRENT EXPLOSIVE ENEMIES: %i", mCurrentExplosiveEnemies);
	LOG("CURRENT ENEMIES: %i", mCurrentEnemies);

	if (mTotalNumEnemies <= 0 && mCurrentEnemies <= 0 && mCurrentTraps <= 0)
	{
		ActivateArea(false);

		//MG
		mHasBeenActivated = false;

		mGameObject->SetEnabled(false);
		return;
	}
	//LOG("REMAINING ENEMIES: %i", mTotalNumEnemies);
}

inline void BattleArea::ActivateArea(bool activate)
{

	CloseDoors(activate);

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

	SetTrapState(mTrap1, activate);
	SetTrapState(mTrap2, activate);
	SetTrapState(mTrap3, activate);
	SetTrapState(mTrap4, activate);

	if (!activate)
	{
		GameManager::GetInstance()->SetActiveBattleArea(nullptr);
		GameManager::GetInstance()->GetHud()->SetDialog();
		//LOG("Sanity");
	}
}

void BattleArea::OnCollisionEnter(CollisionData* collisionData)
{
	if (collisionData->collidedWith->GetTag().compare("Player") == 0 && !mHasBeenActivated)
	{
		mHasBeenActivated = true;
		GameManager::GetInstance()->SetActiveBattleArea(this);
		ActivateArea(true);
		//LOG("PLAYER COLLISION");
	}
}

void BattleArea::AddExplosiveEnemy()
{
	mCurrentExplosiveEnemies++;
}

void BattleArea::SetTrapState(GameObject* trap, bool enable)
{
	if (trap)
	{
		auto scriptComponent = reinterpret_cast<ScriptComponent*>(trap->GetComponent(ComponentType::SCRIPT));
		if (scriptComponent)
		{
			if (enable)
			{
				scriptComponent->Enable();
			}
			else
			{
				scriptComponent->Disable();
			}
		}
	}
}

void BattleArea::UpdateTrapNumber()
{
	GameObject* trapsArray[] = { mTrap1, mTrap2, mTrap3, mTrap4 };

	for (GameObject* trap : trapsArray)
	{
		if (trap != nullptr)
		{
			mCurrentTraps++;
		}
	}
}

void BattleArea::CloseDoors(bool close)
{
	std::string trigger = (close) ? "tClose" : "tOpen";

	if (mDoor1)
	{
		AnimationComponent* doorAnimation1 = static_cast<AnimationComponent*>(mDoor1->GetComponent(ComponentType::ANIMATION));
		if (doorAnimation1)
		{
			doorAnimation1->SetIsPlaying(true);
			doorAnimation1->SendTrigger(trigger, 0.6f);
			
		}

		BoxColliderComponent* door1Collider = static_cast<BoxColliderComponent*>(mDoor1->GetComponent(ComponentType::BOXCOLLIDER));
		if (door1Collider)
		{
			door1Collider->SetEnable(close);
		}
	}
	if (mDoor2)
	{
		AnimationComponent* doorAnimation2 = static_cast<AnimationComponent*>(mDoor2->GetComponent(ComponentType::ANIMATION));
		if (doorAnimation2)
		{
			doorAnimation2->SetIsPlaying(true);
			doorAnimation2->SendTrigger(trigger, 0.6f);
		}

		BoxColliderComponent* door2Collider = static_cast<BoxColliderComponent*>(mDoor2->GetComponent(ComponentType::BOXCOLLIDER));
		if (door2Collider)
		{
			door2Collider->SetEnable(close);
		}
	}

}
