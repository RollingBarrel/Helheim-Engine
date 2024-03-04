#include "Script.h"

Script::Script(GameObject* owner) : mGameObject(owner)
{

}

ScriptAttribute::ScriptAttribute(const char* name, ScriptAttributeType type, void* value) : 
	mVariableName(name), mAttributeType(type), mValue(value)
{
}
