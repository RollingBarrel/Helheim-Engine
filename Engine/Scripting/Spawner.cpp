#include "Spawner.h"
#include "Application.h"
#include "GameObject.h"
#include "ScriptComponent.h"

#include "GameManager.h"

CREATE(Spawner)
{
	CLASS(owner);
	MEMBER(MemberType::INT, mPoolType);
	MEMBER(MemberType::FLOAT, mSpawnRate);
	END_CREATE;
}

Spawner::Spawner(GameObject* owner) : Script(owner) {}

void Spawner::Start()
{
	mPoolManager = GameManager::GetInstance()->GetPoolManager();
}

void Spawner::Update()
{
	if (mIsActive)
	{
		mLastSpawnTime += App->GetDt();
	}
}

bool Spawner::Spawn()
{
	if (mIsActive)
	{
		if (mLastSpawnTime >= mSpawnRate)
		{
			GameObject* enemy = mPoolManager->Spawn(mPoolType);
			if (enemy)
			{
				ScriptComponent* script = reinterpret_cast<ScriptComponent*>(enemy->GetComponent(ComponentType::SCRIPT));
				Enemy* enemyScript = reinterpret_cast<Enemy*>(script->GetScriptInstance());
 				enemyScript->Init();
				enemy->SetWorldPosition(mGameObject->GetWorldPosition());
				enemy->SetEnabled(true);

				mLastSpawnTime = 0.0f;
				return true;
			}
		}
	}
	return false;
}
