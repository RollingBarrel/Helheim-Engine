#include "ResourceScript.h"

ScriptAttribute::ScriptAttribute()
{
	mAttributeType = ScriptAttributeType::None;
	mValue = nullptr;
	mRangeMax = 0;
	mRangeMin = 0;
}

ScriptAttribute::ScriptAttribute(const char* name, ScriptAttributeType type, void* value) :
	mVariableName(name), mAttributeType(type), mValue(value)
{
}

ScriptAttribute::~ScriptAttribute()
{
}

bool ScriptAttribute::setTypeFromString(const std::string& type)
{
	if (type == "int") mAttributeType = ScriptAttributeType::Int;
	else if (type == "float") mAttributeType = ScriptAttributeType::Float;
	else if (type == "double") mAttributeType = ScriptAttributeType::Double;
	else if (type == "std::string") mAttributeType = ScriptAttributeType::String;
	else if (type == "bool") mAttributeType = ScriptAttributeType::Bool;
	else if (type == "float3") mAttributeType = ScriptAttributeType::Float3;
	else return false;
	return true;
}

ResourceScript::ResourceScript(unsigned int uid) :
	Resource(uid, Type::Script)
{
}

ResourceScript::~ResourceScript()
{
}

void ResourceScript::addAttribute(const ScriptAttribute& attribute)
{
	mAttributes.push_back(attribute);
}

void ResourceScript::addDisplayAttribute(const ScriptAttribute& attribute)
{
	mDisplayAttributes.push_back(attribute);
}
