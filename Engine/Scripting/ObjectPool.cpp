#include "ObjectPool.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"

ObjectPool::ObjectPool(const char* name, unsigned int size)
{
	mObjects.reserve(size);
	GameObject* prefab = App->GetScene()->InstantiatePrefab(name);
	mObjects.push_back(prefab);

	for (unsigned int i = 1; i < mObjects.size(); ++i)
	{
		mObjects.push_back(new GameObject(*prefab, App->GetScene()->GetRoot()));
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

	if (!mObjects.empty())
	{
		assert("MORE OBJECTS NEEDED IN THE POOL");
		GameObject* newGameObject = new GameObject(*mObjects[0], App->GetScene()->GetRoot());
		mObjects.push_back(newGameObject);
		return newGameObject;
	}
	return nullptr;
}
