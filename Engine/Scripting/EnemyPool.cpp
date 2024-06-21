#include "EnemyPool.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
CREATE(EnemyPool) {
	CLASS(owner);
	END_CREATE;
}
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
	for (int i = 0; i < static_cast<int>(EnemyType::COUNT); ++i)
	{
		const char* enemyType = GetPrefabFromEnemyType(static_cast<EnemyType>(i));
		
		mEnemies[i].reserve(mNumOfEnemies);
		for (int j = 0; j < mNumOfEnemies; j++)
		{
			GameObject* newEnemy = App->GetScene()->InstantiatePrefab(enemyType); //TODO: Copy prefabs instead of instantiating all enemies
			if (newEnemy)
			{
				newEnemy->SetParent(mGameObject);
				newEnemy->SetPosition(float3::zero);
				newEnemy->SetEnabled(false);
				std::string newName = newEnemy->GetName() + std::to_string(j);
				newEnemy->SetName(newName.c_str());
				mEnemies[i].push_back(newEnemy);
			}
		}
	}
}

const char* EnemyPool::GetPrefabFromEnemyType(EnemyType enemyType)
{
	switch (enemyType)
	{
	case EnemyType::ROBOT_MELEE:
		return "Robot_Melee.prfb";
	case EnemyType::ROBOT_RANGE:
		return "Robot_Range.prfb";
	}
	return "";
}

void EnemyPool::Update()
{
}

GameObject* EnemyPool::GetEnemy(EnemyType type) const
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
