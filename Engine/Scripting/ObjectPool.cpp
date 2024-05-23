#include "ObjectPool.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "Bullet.h"
#include "ScriptComponent.h"


CREATE(ObjectPool)
{
    CLASS(owner);

    END_CREATE;
}

ObjectPool::ObjectPool(GameObject* owner) : Script(owner) {}

void ObjectPool::Start()
{
    App->GetScene()->FindGameObjectsWithTag(App->GetScene()->GetTagByName("Bullet")->GetID(), mObject);
    mPoolCount = mObject.size();
    for (int i = 0 ; i < mObject.size() ; i++)
    {
        ScriptComponent* script = (ScriptComponent*)mObject[i]->GetComponent(ComponentType::SCRIPT);
        ((Bullet*)script->GetScriptInstance())->objectPool = this;
		mObject[i]->SetEnabled(false);
	}
}

GameObject* ObjectPool::GetPooledObject()
{
    // If the pool count reaches zero, bad
    if (mObject.size() <= 0)
    {
        return nullptr;
    }

    for (int i = 0; i < mObject.size(); i++)
    {
        mObject[i]->SetEnabled(true);
        
        GameObject* goToReturn = mObject[i];

        ScriptComponent* script = (ScriptComponent*)mObject[i]->GetComponent(ComponentType::SCRIPT);
        ((Bullet*)script->GetScriptInstance())->mTimePassed = 0;

        Pop(mObject,i);

        //mPoolCount--;
        return goToReturn;
    }
    
    return nullptr;
}

void ObjectPool::ReturnToPool(GameObject* go) {
    go->SetEnabled(false);
    mObject.push_back(go);
}

void ObjectPool::Update()
{
    /*for (int i = 0; i < mObject.size(); i++)
    {
        if (mObject[i]->IsEnabled())
        {
            mObject[i]->Update();
		}
	}*/
}

/*void ObjectPool::RefillPool()
{
    for (int i = 0; i < mObject.size(); i++)
    {
        mObject[i]->SetEnabled(false); // Disable all objects in the pool
    }
    mPoolCount = mObject.size(); // Reset pool count
}*/

void ObjectPool::Pop(std::vector<GameObject*>& vec, size_t index) {
    if (index >= vec.size()) {
        return;
    }

    vec.erase(vec.begin() + index); // Remove the element at the given index

    // Shift the remaining elements to fill the vacated space
    //for (size_t i = index; i < vec.size(); ++i) {
    //    vec[i] = vec[i + 1];
    //}
    ///vec.pop_back(); // Remove the last element (which is now duplicated at the end)
}

void ObjectPool::RefillPool()
{
}
