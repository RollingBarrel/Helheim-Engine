#pragma once
#include "Globals.h"
#include <vector>

class GameObject;


class ENGINE_API Script
{
public:

	Script(GameObject* owner);
	virtual ~Script() {}

	virtual void Start() = 0;
	virtual void Update() = 0;

protected:

	GameObject* mGameObject = nullptr;

private:

	const char* name = nullptr;


};

