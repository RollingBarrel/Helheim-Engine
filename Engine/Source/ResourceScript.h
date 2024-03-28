#pragma once
#include "Resource.h"
#include "Script.h"
#include <vector>

class ScriptAttribute 
{
public:
	ScriptAttribute() {}
	ScriptAttribute(std::string name, VariableType type, void* data) : mName(name), mType(type) {}
	bool setTypeFromString(const std::string& type);
	void setName(const std::string name);
	void setType(const VariableType type);
	size_t sizeOfScriptVariable() const;
	const std::string GetName() const { return mName; }
	const VariableType GetType() const { return mType; }

private:
	std::string mName;
	VariableType mType;

};

class ResourceScript : public Resource
{
public:
	ResourceScript(unsigned int uid);
    ~ResourceScript();

	void addAttribute(const ScriptAttribute& attribute);

	const std::vector<ScriptAttribute>& GetAttributes() const { return mAttributes; }
	const std::string& GetScriptName() const { return mScriptName; }

	void SetScriptName(const std::string& scriptName) { mScriptName = scriptName; }

private:
	std::string mScriptName;
	std::vector<ScriptAttribute> mAttributes;
};

