#pragma once
#include "Macros.h"
#include "Script.h"


GENERATE_BODY(ObjectPool);
class ObjectPool : public Script
{
	FRIEND(ObjectPool)

public:
	//make that object pool has to be initializided with the owner ans the amount of objects to pool
	ObjectPool(GameObject* owner);
	GameObject* GetPooledObject();
	void ReturnToPool(GameObject* go);
	void Start() override;
	void Update() override;


private:
	void Pop(std::vector<GameObject*>& vec, size_t index);

	//list of objects
	std::vector<GameObject*> mObject;
	GameObject* bulletPrefab = nullptr;
	int mPoolCount = 0;
	void RefillPool();

};

