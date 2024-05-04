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
	void Start() override;
	void Update() override;


private:

	//list of objects
	std::vector<GameObject*> mObject;
	GameObject* bulletPrefab = nullptr;
	int mPoolCount = 0;
	void RefillPool();

};

