#pragma once
#include "Globals.h"
#include <vector>

class GameObject;

class ENGINE_API ScriptAttribute
{
public:
	enum class ScriptAttributeType
	{
		None, 
		Int, 
		Float, 
		Double, 
		String, 
		Float3,
	};
	ScriptAttribute(const char* name, ScriptAttributeType type, void* value);
	~ScriptAttribute();
private:
	const char* mVariableName;
	ScriptAttributeType mAttributeType;
	void* mValue;
};

class ENGINE_API Script
{
public:

	Script(GameObject* owner);
	virtual ~Script() {}

	virtual void Start() = 0;
	virtual void Update() = 0;

	
	
protected:

	GameObject* mGameObject = nullptr;
	std::vector<ScriptAtribute> listOfAtributes;

private:

	const char* name = nullptr;


};

