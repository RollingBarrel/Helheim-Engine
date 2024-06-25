#include "EnemySpawner.h"
#include "EnemyPool.h"
#include "Application.h"
#include "GameObject.h"
#include "GameManager.h"
#include "ScriptComponent.h"

CREATE(EnemySpawner)
{
	CLASS(owner);
	SEPARATOR("Enemy Spawner");
	MEMBER(MemberType::INT, mEnemyType);
	MEMBER(MemberType::FLOAT, mSpawnRate);
	END_CREATE;
}

EnemySpawner::EnemySpawner(GameObject* owner) : Script(owner) {}

void EnemySpawner::Start()
{
	mEnemyPool = reinterpret_cast<EnemyPool*>(reinterpret_cast<ScriptComponent*>(GameManager::GetInstance()->GetEnemyPool()->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
}

void EnemySpawner::Update()
{
	if (mIsActive)
	{
		mLastSpawnTime += App->GetDt();
	}
}

bool EnemySpawner::Spawn()
{
	if (mIsActive)
	{
		if (mLastSpawnTime >= mSpawnRate)
		{
			GameObject* enemy = mEnemyPool->GetEnemy(mEnemyType);
			if (enemy)
			{
				ScriptComponent* script = reinterpret_cast<ScriptComponent*>(enemy->GetComponent(ComponentType::SCRIPT));
				Enemy* enemyScript = reinterpret_cast<Enemy*>(script->GetScriptInstance());
				enemyScript->Reset();
				enemy->SetWorldPosition(mGameObject->GetWorldPosition());
				enemy->SetEnabled(true);

				mLastSpawnTime = 0.0f;
				return true;
			}
		}
	}
	return false;
}
