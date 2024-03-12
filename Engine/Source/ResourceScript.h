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
		Bool,
		Float3,
	};
	ScriptAttribute();
	ScriptAttribute(const char* name, ScriptAttributeType type, void* value);
	~ScriptAttribute();
	bool setTypeFromString(const std::string& type);
	void setName(const std::string& name) { mVariableName = name; }
	//void setTooltip(const std::string& tooltip) { mTooltip = tooltip; }
	void setRangeMin(const int min) { mRangeMin = min; }
	void setRangeMax(const int max) { mRangeMax = max; }
	//void setHeader(const std::string& header) { mHeader = header; }
private:
	std::string mVariableName;
	ScriptAttributeType mAttributeType;
	void* mValue;
	//std::string mTooltip;
	int mRangeMin;
	int mRangeMax;
	//std::string mHeader;
};

class ResourceScript : public Resource
{
public:
	ResourceScript(unsigned int uid);
    ~ResourceScript();

	void addAttribute(const ScriptAttribute& attribute);
	void addDisplayAttribute(const ScriptAttribute& attribute);

	const std::vector<ScriptAttribute>& GetAttributes() const { return mAttributes; }
	const std::vector<ScriptAttribute>& GetDisplayAttributes() const  { return mDisplayAttributes; }
	const std::string& GetScriptName() const { return mScriptName; }

	void SetScriptName(const std::string& scriptName) { mScriptName = scriptName; }

private:
	std::string mScriptName;
	std::vector<ScriptAttribute> mAttributes;
	std::vector<ScriptAttribute> mDisplayAttributes;
};

