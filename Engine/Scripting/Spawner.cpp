#include "Spawner.h"
#include "Application.h"
#include "GameObject.h"
#include "ScriptComponent.h"
#include "ParticleSystemComponent.h"
#include "GameManager.h"

CREATE(Spawner)
{
	CLASS(owner);
	MEMBER(MemberType::INT, mPoolType);
	MEMBER(MemberType::FLOAT, mParticlesTime);
	MEMBER(MemberType::GAMEOBJECT, mParticlesGO);
	MEMBER(MemberType::BOOL, mOnlyOnce);
	MEMBER(MemberType::INT, mEnemiesPerRound);
	END_CREATE;
}

Spawner::Spawner(GameObject* owner) : Script(owner) {}

void Spawner::Start()
{
	mPoolManager = GameManager::GetInstance()->GetPoolManager();
	mParticlesOn = true;
}

void Spawner::Update()
{
	
	if (mSpawnedCounter>0)
	{
		if (mParticlesOn and mParticlesGO)
		{
			mParticlesGO->SetEnabled(false);
			mParticlesGO->SetEnabled(true);
			mParticlesOn = false;
		}

		if (mSpawnDelayTimer.Delay(mSpawnDelay))
		{			
			GameObject* enemy = mPoolManager->Spawn(mPoolType);
			if (enemy)
			{
				ScriptComponent* script = static_cast<ScriptComponent*>(enemy->GetComponent(ComponentType::SCRIPT));
				Enemy* enemyScript = static_cast<Enemy*>(script->GetScriptInstance());
				enemy->SetWorldPosition(mGameObject->GetWorldPosition());
				enemy->SetEnabled(true);
				enemyScript->Init();

				if(mOnlyOnce)
				{
					mIsActive = false;
					mSpawnedCounter= 0;
					return;
				}
				--mSpawnedCounter;
				mParticlesOn = true;
			}
		}
	}
}

bool Spawner::Spawn()
{
	if (mIsActive)
	{	
		++mSpawnedCounter;
		return true;
	}
	return false;
}

