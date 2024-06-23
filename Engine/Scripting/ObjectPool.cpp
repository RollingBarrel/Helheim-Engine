#include "ObjectPool.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"

ObjectPool::ObjectPool(const char* PrefabFileName, unsigned int size, GameObject* parent)
{
	if (!parent)
	{
		parent = App->GetScene()->GetRoot();
	}
	

	mObjects.reserve(size);
	GameObject* prefab = App->GetScene()->InstantiatePrefab(PrefabFileName, parent);
	prefab->SetEnabled(false);
	mObjects.push_back(prefab);
	
	for (unsigned int i = 1; i < size; ++i)
	{
		GameObject* duplicatedGameObject = App->GetScene()->DuplicateGameObject(prefab);
		duplicatedGameObject->SetEnabled(false);
		mObjects.push_back(duplicatedGameObject);
	}
}

ObjectPool::~ObjectPool()
{
}

GameObject* ObjectPool::GetObject()
{
	for (unsigned int i = 0; i < mObjects.size(); ++i)
	{
		if (!mObjects[i]->IsEnabled())
		{
			mObjects[i]->SetEnabled(true);
			return mObjects[i];
		}
	}

	if (mObjects.empty())
	{
		assert(false && "MORE OBJECTS NEEDED IN THE POOL");
		GameObject* newGameObject = new GameObject(*mObjects[0], App->GetScene()->GetRoot());
		mObjects.push_back(newGameObject);
		return newGameObject;
	}
	return nullptr;
}
