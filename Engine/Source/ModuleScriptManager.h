#pragma once
#include "Module.h"
#include <vector>

class ScriptComponent;
struct Member;

class ModuleScriptManager :	public Module
{
public:

	ModuleScriptManager();
	~ModuleScriptManager();

	virtual bool Init() override;
	virtual update_status PreUpdate(float dt) override {};
	virtual update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;
	
	void AddScript(ScriptComponent* script);
	void RemoveScript(ScriptComponent* script);

protected:
	std::vector<ScriptComponent*> mScripts;
	void* mHandle = nullptr;
};

