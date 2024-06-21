#include "Application.h"

class ModuleDebugDraw;
class ModuleEngineCamera;
class ModuleEditor;
class ModuleEngineResource;
class ModuleEngineInput;
class ModuleEngineScriptManager;
class NavMeshController;

class EngineApplication : public Application
{
public:
	EngineApplication();
	~EngineApplication();

	bool Init();
	update_status Update(float dt);
	bool CleanUp();

	Timer* GetEngineClock() const { return mEngineTimer; }
	Timer* GetGameClock() const { return mGameTimer; }
	void SetCurrentClock(Timer* clock) { mCurrentTimer = clock; }

	ModuleEngineResource* GetEngineResource() { return engineResources; }
	ModuleEngineInput* GetEngineInput() { return engineInput; }
	ModuleEngineCamera* GetEngineCamera() { return engineCamera; }
	ModuleDebugDraw* GetDebugDraw() { return debugDraw; }
	ModuleEngineScriptManager* GetEngineScriptManager() { return engineScriptManager; }
	ModuleEditor* GetEditor() { return editor; }
	NavMeshController* GetNavController() { return mNavMeshController; }

	void Start();
	void Stop();

	float GetRealDt() const;
	float GetGameDt() const;

private:
	ModuleDebugDraw* debugDraw = nullptr;
	ModuleEngineCamera* engineCamera = nullptr;
	ModuleEditor* editor = nullptr;
	ModuleEngineResource* engineResources = nullptr;
	ModuleEngineInput* engineInput = nullptr;
	ModuleEngineScriptManager* engineScriptManager = nullptr;
	NavMeshController* mNavMeshController;


#define NUM_MODULES 15
	Module* modules[NUM_MODULES];

	Timer* mEngineTimer = nullptr;
	Timer* mGameTimer = nullptr;
};

extern EngineApplication* EngineApp;