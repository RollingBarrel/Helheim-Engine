#pragma once
#include "Module.h"
#include <vector>
#include <unordered_map>

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
	
	void RemoveGameObject(GameObject* gameObject);
	void AddScript(ScriptComponent* script);
	void RemoveScript(ScriptComponent* script);
	void* GetDLLHandle() { return mHandle; }
	void AwakeScripts();
	void StartScripts();
	void AwakeGameObjectScripts(GameObject* gameobject);
	void StartGameObjectScripts(GameObject* gameobject);
	

	void AddGameObjectToMap(GameObject** gameObject);
	void RemoveGameObjectFromMap(GameObject** gameObject);

protected:
	std::vector<ScriptComponent*> mScripts;
	std::unordered_map<unsigned int, std::vector<GameObject**>> mGameObjectsPointersMap;

	void* mHandle = nullptr;
	bool mFirstFrame = true;

};

