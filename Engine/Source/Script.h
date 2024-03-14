#pragma once
#include "Globals.h"
#include <vector>
class GameObject;

enum class  VariableType : int
{
	// IF THE NUMBERS CHANGES THE SCRIPTS MAY BREAK
	INT = 0, 
	FLOAT = 1, 
	BOOL = 2, 
	GAMEOBJECT = 3, 
	FLOAT3 = 4, 
	FLOAT3X3 = 5
};

struct ScriptVariable {

	const char* mName;
	VariableType mType;
	void* mData;

	ScriptVariable() {}
	ScriptVariable(const char* name, VariableType type, void* data) : mName(name), mType(type), mData(data) {}
};

class ENGINE_API Script
{
	friend class InspectorPanel;
	friend class ScriptComponent;

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

