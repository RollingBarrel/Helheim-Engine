#pragma once
#include "Module.h"
#include <vector>

class Script;


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
	void AddScript(Script* script) { mScripts.push_back(script); }
	void Play();
	void Stop();

	void Start();

private:

	std::vector<Script*> mScripts;
	void* mHandle = nullptr;
	bool mIsPlaying = false;
};

