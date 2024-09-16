#include "BattleArea.h"
#include "ScriptComponent.h"
#include "BoxColliderComponent.h"
#include "AnimationComponent.h"
#include "GameObject.h"

#include "GameManager.h"
#include "AudioManager.h"
#include "HudController.h"
#include "Spawner.h"
#include "EnemyExplosiveSpawner.h"

CREATE(BattleArea)
{
	CLASS(owner);

	MEMBER(MemberType::GAMEOBJECT, mSpawnerGO1);
	MEMBER(MemberType::GAMEOBJECT, mSpawnerGO2);
	MEMBER(MemberType::GAMEOBJECT, mSpawnerGO3);
	MEMBER(MemberType::GAMEOBJECT, mSpawnerGO4);
	MEMBER(MemberType::INT, mWavesRounds);
	SEPARATOR("DOORS");
	MEMBER(MemberType::GAMEOBJECT, mDoor1);
	MEMBER(MemberType::GAMEOBJECT, mDoor2);
	MEMBER(MemberType::GAMEOBJECT, mElevator);
	SEPARATOR("EXPLOSIVE SPAWNERS");
	MEMBER(MemberType::GAMEOBJECT, mExplosiveSpawn1);
	MEMBER(MemberType::GAMEOBJECT, mExplosiveSpawn2);
	MEMBER(MemberType::GAMEOBJECT, mExplosiveSpawn3);
	MEMBER(MemberType::GAMEOBJECT, mExplosiveSpawn4);
	SEPARATOR("TUTORIAL");
	MEMBER(MemberType::BOOL, mIsTutorialArea);
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
	mSpawners.reserve(4);
	if (mSpawnerGO1)
	{
		mEnemySpawner1 = static_cast<Spawner*>(static_cast<ScriptComponent*>(mSpawnerGO1->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
		mSpawners.push_back(mEnemySpawner1);

	}
	if (mSpawnerGO2)
	{
		mEnemySpawner2 = static_cast<Spawner*>(static_cast<ScriptComponent*>(mSpawnerGO2->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
		mSpawners.push_back(mEnemySpawner2);
	}
	if (mSpawnerGO3)
	{
		mEnemySpawner3 = static_cast<Spawner*>(static_cast<ScriptComponent*>(mSpawnerGO3->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
		mSpawners.push_back(mEnemySpawner3);
	}
	if (mSpawnerGO4)
	{
		mEnemySpawner4 = static_cast<Spawner*>(static_cast<ScriptComponent*>(mSpawnerGO4->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
		mSpawners.push_back(mEnemySpawner4);
	}
	if (mExplosiveSpawn1)
	{
		mEnemyExplosiveSpawner1 = static_cast<EnemyExplosiveSpawner*>(static_cast<ScriptComponent*>(mExplosiveSpawn1->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
		mExplosiveSpawners.push_back(mEnemyExplosiveSpawner1);
	}
	if (mExplosiveSpawn2)
	{
		mEnemyExplosiveSpawner2 = static_cast<EnemyExplosiveSpawner*>(static_cast<ScriptComponent*>(mExplosiveSpawn2->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
		mExplosiveSpawners.push_back(mEnemyExplosiveSpawner2);
	}
	if (mExplosiveSpawn3)
	{
		mEnemyExplosiveSpawner3 = static_cast<EnemyExplosiveSpawner*>(static_cast<ScriptComponent*>(mExplosiveSpawn3->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
		mExplosiveSpawners.push_back(mEnemyExplosiveSpawner3);
	}
	if (mExplosiveSpawn4)
	{
		mEnemyExplosiveSpawner4 = static_cast<EnemyExplosiveSpawner*>(static_cast<ScriptComponent*>(mExplosiveSpawn4->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
		mExplosiveSpawners.push_back(mEnemyExplosiveSpawner4);
	};

	mCollider = static_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
	if (mCollider)
	{
		mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&BattleArea::OnCollisionEnter, this, std::placeholders::_1)));
	}

	CloseDoors(false);
}

void BattleArea::Update()
{
	if (mHasBeenActivated && mNeedsToSpawn)
	{
		for (size_t i = 0; i < mSpawners.size(); i++)
		{
			if (mSpawners[i]->IsActive())
			{
				for (size_t y = 0; y < mSpawners[i]->GetEnemiesPerRound(); y++)
				{
					mSpawners[i]->Spawn();
					mCurrentEnemies++;
				}
			}
		}
		for (size_t i = 0; i < mExplosiveSpawners.size(); i++)
		{
			if (mExplosiveSpawners[i]->IsActive())
			{
				for (size_t y = 0; y < mExplosiveSpawners[i]->GetEnemiesPerRound(); y++)
				{
					mExplosiveSpawners[i]->Spawn();
					mCurrentEnemies++;
				}
			}
		}
		mNeedsToSpawn = false;
	}
}

void BattleArea::EnemyDestroyed(GameObject* enemy)
{
	mCurrentEnemies--;
	if (mCurrentEnemies == 0)
	{
		mWavesRounds--;
		if (mWavesRounds >= 1)
		{
			mNeedsToSpawn = true;
		}
	}

	if (mWavesRounds <= 0)
	{
		ActivateArea(false);
		mGameObject->SetEnabled(false);
		return;
	}
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
	if (mEnemyExplosiveSpawner1)
	{
		mEnemyExplosiveSpawner1->Active(activate);
	}
	if (mEnemyExplosiveSpawner2)
	{
		mEnemyExplosiveSpawner2->Active(activate);
	}
	if (mEnemyExplosiveSpawner3)
	{
		mEnemyExplosiveSpawner3->Active(activate);
	}
	if (mEnemyExplosiveSpawner4)
	{
		mEnemyExplosiveSpawner4->Active(activate);
	}
	mNeedsToSpawn = activate;

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
		if (mIsTutorialArea)
		{
			GameManager::GetInstance()->ActivateSecondTutorial();
		}
 		mHasBeenActivated = true;
		GameManager::GetInstance()->SetActiveBattleArea(this);
		ActivateArea(true);
		//LOG("PLAYER COLLISION");
	}
}

void BattleArea::CloseDoors(bool close)
{
	std::string trigger = (close) ? "tClose" : "tOpen";

	if (mDoor1)
	{
		GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::DOOR, mDoor1->GetWorldPosition());

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
		GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::DOOR, mDoor2->GetWorldPosition());

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

	if (mElevator)
	{
		mElevator->GetComponent(ComponentType::SCRIPT)->SetEnable(!close);
	}

}
