#include "ObjectPool.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "Bullet.h"



CREATE(ObjectPool)
{
    CLASS(owner);

    END_CREATE;
}

ObjectPool::ObjectPool(GameObject* owner) : Script(owner) {}

void ObjectPool::Start()
{
    App->GetScene()->FindGameObjectsWithTag(App->GetScene()->GetRoot(), App->GetScene()->GetTagByName("Bullet")->GetID(), mObject);
    mPoolCount = mObject.size();
    for (int i = 0 ; i < mObject.size() ; i++)
    {
		mObject[i]->SetEnabled(false);
	}
}

GameObject* ObjectPool::GetPooledObject()
{
    for (int i = 0; i < mObject.size(); i++)
    {
        if (!mObject[i]->IsEnabled())
        {
            mObject[i]->SetEnabled(true);
             mPoolCount--;
            return mObject[i];
        }
    }
    // If the pool count reaches zero, refill the pool
    if (mPoolCount <= 0)
    {
        RefillPool();

        if (!mObject.empty())
        {
            mObject[0]->SetEnabled(true);
            mPoolCount--; // Decrease the pool count
            return mObject[0];
        }
    }
    return nullptr;
}

void ObjectPool::Update()
{
    for (int i = 0; i < mObject.size(); i++)
    {
        if (mObject[i]->IsEnabled())
        {
            mObject[i]->Update();
		}
	}
}

void ObjectPool::RefillPool()
{
    for (int i = 0; i < mObject.size(); i++)
    {
        mObject[i]->SetEnabled(false); // Disable all objects in the pool
    }
    mPoolCount = mObject.size(); // Reset pool count
}