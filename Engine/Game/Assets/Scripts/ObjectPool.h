#pragma once

#include <vector>

class GameObject;

class ObjectPool
{
public:
	ObjectPool() = delete;
	ObjectPool(const char* PrefabFileName, unsigned int size, GameObject* parent = nullptr);
	GameObject* GetObject();

private:
	std::vector<GameObject*> mObjects;
};

