#pragma once
#include "Resource.h"
#include "Script.h"
#include <vector>


class ResourceScript : public Resource
{
public:
	ResourceScript(unsigned int uid);
    ~ResourceScript();

	const std::string& GetScriptName() const { return mScriptName; }

	void SetScriptName(const std::string& scriptName) { mScriptName = scriptName; }

private:
	std::string mScriptName;
};

