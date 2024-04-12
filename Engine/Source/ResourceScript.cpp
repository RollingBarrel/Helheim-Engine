#include "ResourceScript.h"
#include "float3.h"

bool ScriptAttribute::setTypeFromString(const std::string& type)
{
	if (type == "int") mType = MemberType::INT;
	else if (type == "float") mType = MemberType::FLOAT;
	else if (type == "bool") mType = MemberType::BOOL;
	else if (type == "float3") mType = MemberType::FLOAT3;
	else if (type == "GameObject*") mType = MemberType::GAMEOBJECT;

	else return false;
	return true;
}

void ScriptAttribute::setName(std::string name)
{
	mName = name;
}

void ScriptAttribute::setType(const MemberType type)
{
	mType = type;
}

size_t ScriptAttribute::sizeOfScriptVariable() const
{
	size_t sizeTSizeInBytes = sizeof(size_t);
	size_t nameSizeInBytes = mName.size() * sizeof(char);
	size_t typeSizeInBytes = sizeof(MemberType);
	//switch (mType)
	//{
	//case MemberType::INT:
	//	return nameSizeInBytes + typeSizeInBytes + sizeof(int);
	//case MemberType::FLOAT:
	//	return nameSizeInBytes + typeSizeInBytes + sizeof(float);
	//case MemberType::BOOL:
	//	return nameSizeInBytes + typeSizeInBytes + sizeof(bool);
	//case MemberType::FLOAT3:
	//	return nameSizeInBytes + typeSizeInBytes + sizeof(float3);
	//case MemberType::GAMEOBJECT:
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
