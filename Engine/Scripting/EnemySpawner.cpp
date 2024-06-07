#include "EnemySpawner.h"
#include "Application.h"

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
			// Do instantiation from enemy pool
			mLastSpawnTime = 0.0f;
			--mMaxEnemies;
		}
	}
}
