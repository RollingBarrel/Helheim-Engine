#include "ResourceScript.h"
#include "float3.h"

bool ScriptAttribute::setTypeFromString(const std::string& type)
{
	if (type == "int") mType = VariableType::INT;
	else if (type == "float") mType = VariableType::FLOAT;
	else if (type == "bool") mType = VariableType::BOOL;
	else if (type == "float3") mType = VariableType::FLOAT3;
	else if (type == "GameObject*") mType = VariableType::GAMEOBJECT;

	else return false;
	return true;
}

void ScriptAttribute::setName(std::string name)
{
	mName = name;
}

void ScriptAttribute::setType(const VariableType type)
{
	mType = type;
}

size_t ScriptAttribute::sizeOfScriptVariable() const
{
	size_t sizeTSizeInBytes = sizeof(size_t);
	size_t nameSizeInBytes = mName.size() * sizeof(char);
	size_t typeSizeInBytes = sizeof(VariableType);
	//switch (mType)
	//{
	//case VariableType::INT:
	//	return nameSizeInBytes + typeSizeInBytes + sizeof(int);
	//case VariableType::FLOAT:
	//	return nameSizeInBytes + typeSizeInBytes + sizeof(float);
	//case VariableType::BOOL:
	//	return nameSizeInBytes + typeSizeInBytes + sizeof(bool);
	//case VariableType::FLOAT3:
	//	return nameSizeInBytes + typeSizeInBytes + sizeof(float3);
	//case VariableType::GAMEOBJECT:
	//	return nameSizeInBytes + typeSizeInBytes + sizeof(GameObject*);
	//default:
	//	return 0;
	//}
	return sizeTSizeInBytes + nameSizeInBytes + typeSizeInBytes;
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
