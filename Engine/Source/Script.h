#pragma once
#include "Globals.h"
#include <vector>
#include <string>
#include "Physics.h"
#include "Debug.h"

class GameObject;

enum class  VariableType : int
{
	// IF THE NUMBERS CHANGES THE SCRIPTS MAY BREAK
	INT = 0, 
	FLOAT = 1, 
	BOOL = 2, 
	GAMEOBJECT = 3, 
	FLOAT3 = 4, 
};

struct ScriptVariable {

	const char* mName = nullptr;
	VariableType mType;
	void* mData = nullptr;

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
	void SetName(const std::string name);

	virtual void Start() = 0;
	virtual void Update() = 0;

	//virtual void OnButtonClick() = 0;

protected:

	GameObject* mGameObject = nullptr;

private:

	std::string mName;
};

