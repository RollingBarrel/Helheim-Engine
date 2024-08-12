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
	MEMBER(MemberType::FLOAT, mSpawnDelay);
	END_CREATE;
}

Spawner::Spawner(GameObject* owner) : Script(owner) {}

void Spawner::Start()
{
	mPoolManager = GameManager::GetInstance()->GetPoolManager();

}

void Spawner::Update()
{

	if (mSpawnedCounter>0)
	{
		if (mParticlesGO)
		{
			mParticlesGO->SetEnabled(true);
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
				if (mParticlesGO)
				{
					mParticlesGO->SetEnabled(false);
				}
				--mSpawnedCounter;
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

