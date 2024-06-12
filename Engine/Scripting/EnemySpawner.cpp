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
	MEMBER(MemberType::FLOAT, mSpawnRate);
	END_CREATE;
}

EnemySpawner::EnemySpawner(GameObject* owner) : Script(owner) {}

void EnemySpawner::Start()
{

}

void EnemySpawner::Update()
{
	if (mIsActive)
	{
		mLastSpawnTime += App->GetDt();
	}
}

void EnemySpawner::Spawn()
{
	if (mIsActive)
	{
		if (mLastSpawnTime >= mSpawnRate)
		{
			GameObject* enemy = GameManager::GetInstance()->GetEnemyPool()->GetEnemy(mEnemyType);
			if (enemy)
			{
				ScriptComponent* script = static_cast<ScriptComponent*>(enemy->GetComponent(ComponentType::SCRIPT));
				Enemy* enemyScript = static_cast<Enemy*>(script->GetScriptInstance());
				enemyScript->Reset();
				enemy->SetPosition(mGameObject->GetPosition());
				enemy->SetEnabled(true);

				mLastSpawnTime = 0.0f;
			}
		}
	}
}
