#include "ResourceScript.h"

ScriptAttribute::ScriptAttribute()
{
	mVariableName = nullptr;
	mAttributeType = ScriptAttributeType::None;
	mValue = nullptr;
}

ScriptAttribute::ScriptAttribute(const char* name, ScriptAttributeType type, void* value) :
	mVariableName(name), mAttributeType(type), mValue(value)
{
}

ScriptAttribute::~ScriptAttribute()
{
}



ResourceScript::ResourceScript(unsigned int uid, ScriptAttribute* scriptAttributes) : 
	Resource(uid, Type::Script), mScriptAttributes(scriptAttributes)
{
}

ResourceScript::~ResourceScript()
{
}
