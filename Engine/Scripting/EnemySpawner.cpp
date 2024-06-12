#include "EnemySpawner.h"
#include "Application.h"
//#include "RoomController.h"

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
		if (mLastSpawnTime >= mSpawnRate && mMaxEnemies > 0)
		{
			// Instantiate from enemy pool
			GameObject* enemy = nullptr; //RoomController->GetEnemyPool()->GetEnemy(mEnemyType);
			if (enemy)
			{
				enemy->SetPosition(mGameObject->GetPosition());
				enemy->SetEnabled(true);

				mLastSpawnTime = 0.0f;
				--mMaxEnemies;
			}
		}
	}
}
