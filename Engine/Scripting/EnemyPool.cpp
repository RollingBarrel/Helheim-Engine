#include "EnemyPool.h"
#include "GameObject.h"

EnemyPool::EnemyPool(GameObject* owner) : Script(owner) {}

EnemyPool::~EnemyPool()
{
	for (auto& enemyVector : mEnemies)
	{
		enemyVector.clear();
	}
}

void EnemyPool::Start()
{
	for (int i = 0; i < mGameObject->GetChildren().size() && i < static_cast<int>(EnemyType::COUNT); ++i)
	{
		GameObject* child = mGameObject->GetChildren()[i];
		mEnemies[i].reserve(child->GetChildren().size());
		for (GameObject* enemyInstance : child->GetChildren())
		{
			mEnemies[i].push_back(enemyInstance);
		}
	}
}

void EnemyPool::Update()
{
}

GameObject* EnemyPool::GetEnemy(EnemyType type)
{
	for (GameObject* enemy : mEnemies[static_cast<int>(type)])
	{
		if (!enemy->IsEnabled())
		{
			return enemy;
		}
	}
	return nullptr;
}
