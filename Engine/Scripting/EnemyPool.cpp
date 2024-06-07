#include "EnemyPool.h"
#include "GameObject.h"
#include "ScriptComponent.h"

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
	std::vector<Component*> components;
	mGameObject->GetComponentsInChildren(ComponentType::SCRIPT, components);

	for (Component* component : components)
	{
		ScriptComponent* scriptComponent = static_cast<ScriptComponent*>(component);

	}
}

void EnemyPool::Update()
{
}

GameObject* EnemyPool::GetEnemy(EnemyType type)
{
	return nullptr;
}
