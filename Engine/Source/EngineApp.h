#include "Application.h"

class ModuleDebugDraw;
class ModuleEditor;
class ModuleEngineResource;
class ModuleEngineInput;
class ModuleEngineScriptManager;

class EngineApplication : public Application
{
public:
	EngineApplication();
	~EngineApplication();

	bool Init();
	update_status Update(float dt);
	bool CleanUp();

	bool IsPlayMode() { return mIsPlayMode; }
	void PlayMode(bool play) { mIsPlayMode = play; }
	Timer* GetEngineClock() const { return mEngineTimer; }
	Timer* GetGameClock() const { return mGameTimer; }
	void SetCurrentClock(Timer* clock) { mCurrentTimer = clock; }

	ModuleEngineResource* GetEngineResource() { return engineResources; }
	ModuleEngineInput* GetEngineInput() { return engineInput; }
	ModuleDebugDraw* GetDebugDraw() { return debugDraw; }
	ModuleEngineScriptManager* GetEngineScriptManager() { return engineScriptManager; }
	ModuleEditor* GetEditor() { return editor; }

	float GetRealDt() const;
	float GetGameDt() const;

private:
	ModuleDebugDraw* debugDraw = nullptr;
	ModuleEditor* editor = nullptr;
	ModuleEngineResource* engineResources = nullptr;
	ModuleEngineInput* engineInput = nullptr;
	ModuleEngineScriptManager* engineScriptManager = nullptr;

#define NUM_MODULES 15
	Module* modules[NUM_MODULES];

	Timer* mEngineTimer = nullptr;
	Timer* mGameTimer = nullptr;
	bool mIsPlayMode = false;
};

extern EngineApplication* EngineApp;