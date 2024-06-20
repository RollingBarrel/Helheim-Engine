#pragma once

#include <vector>

class GameObject;

class ObjectPool
{
public:
	ObjectPool(const char* name, unsigned int size);
	~ObjectPool();
	GameObject* GetObject();

private:
	std::vector<GameObject*> mObjects;
};

