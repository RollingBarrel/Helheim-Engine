#pragma once
#include "Module.h"
#include "ModuleScriptManager.h"
#include <vector>

class ScriptComponent;
struct Member;

class ModuleEngineScriptManager : public ModuleScriptManager
{
public:

	ModuleEngineScriptManager();
	~ModuleEngineScriptManager();

	bool Init() override;
	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;
	
	void Pause(bool pause);
	void StartScripts() override;

private:
	void UpdateScripts();
	void HotReload();
	void ReloadScripts(const std::vector<std::vector<std::pair<Member, void*>>>& oldScripts);
	void SaveOldScript(std::vector<std::vector<std::pair<Member, void*>>>& oldScripts);
	int64_t mLastModificationTime = 0;
	bool mPause = false;
	float mUpdateScriptsTimer = 0.0f;
};

