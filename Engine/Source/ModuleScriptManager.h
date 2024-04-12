#pragma once
#include "Module.h"
#include <vector>

class ScriptComponent;
class Member;

class ModuleScriptManager :	public Module
{
public:

	ModuleScriptManager();
	~ModuleScriptManager();

	bool Init() override;
	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	void* GetDLLHandle() { return mHandle; }
	bool CleanUp() override;
	void AddScript(ScriptComponent* script);
	void RemoveScript(ScriptComponent* script);
	void HotReload();
	

	void Play();
	void Stop();

	void Start();

private:

	void ReloadScripts(const std::vector<std::vector<std::pair<Member, void*>>>& oldScripts);
	void SaveOldScript(std::vector<std::vector<std::pair<Member, void*>>>& oldScripts);
	//std::vector<Script*> mScripts;
	std::vector<ScriptComponent*> mScripts;
	void* mHandle = nullptr;
	bool mIsPlaying = false;
	int64_t mLastModificationTime = 0;
};

