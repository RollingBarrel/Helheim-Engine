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

		mEnemies[i].reserve(mNumOfEnemies);
		for (size_t i = 0; i < mNumOfEnemies; i++)
		{
			std::string enemyType = "";
			switch (static_cast<EnemyType>(i))
			{
			case EnemyType::ROBOT_MELEE:
				enemyType= "Robot_Melee.prfb";
			case EnemyType::ROBOT_RANGE:
				enemyType = "Robot_Range.prfb";

			default:
				break;
			}
			GameObject* newEnemy = App->GetScene()->InstantiatePrefab(enemyType.c_str());
			if (newEnemy) {
				newEnemy->SetPosition(mGameObject->GetPosition());
				mEnemies[i].push_back(newEnemy);
			}

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

GameObject* EnemyPool::CreateEnemyInstance(EnemyType type)
{
	// TODO
	return nullptr;
}