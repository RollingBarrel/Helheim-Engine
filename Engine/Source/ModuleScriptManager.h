#pragma once
#include "Module.h"
#include <vector>

class ScriptComponent;
struct Member;

class ENGINE_API ModuleScriptManager :	public Module
{
public:

	ModuleScriptManager();
	~ModuleScriptManager();

	virtual bool Init() override;
	virtual update_status PreUpdate(float dt) override { return UPDATE_CONTINUE;  }
	virtual update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;
	
	void AddScript(ScriptComponent* script);
	void RemoveScript(ScriptComponent* script);
	void* GetDLLHandle() { return mHandle; }
	void AwakeScripts();
	void AwakeScripts(GameObject* gameObject);
	virtual void StartScripts();


protected:
	std::vector<ScriptComponent*> mScripts;
	void* mHandle = nullptr;
};

