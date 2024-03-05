#pragma once
#include "Resource.h"
#include <vector>

class ScriptAttribute
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
	ScriptAttribute();
	ScriptAttribute(const char* name, ScriptAttributeType type, void* value);
	~ScriptAttribute();
private:
	const char* mVariableName;
	ScriptAttributeType mAttributeType;
	void* mValue;
};

class ResourceScript : public Resource
{
public:
    ResourceScript(unsigned int uid, ScriptAttribute* scriptAttributes);

    ~ResourceScript();

    // Getter functions


private:
    ScriptAttribute* mScriptAttributes;
};

