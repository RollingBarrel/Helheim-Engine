#pragma once
#include "Globals.h"
#include "Module.h"

class ModuleOpenGL;
class ModuleWindow;
class ModuleInput;
class ModuleEditor;
class ModuleRenderExercise;
class ModuleScene;
class ModuleFileSystem;
class ModuleCamera;
class ModuleDebugDraw;
class ModuleUI;

class Timer;
class PreciseTimer;

class Application
{
public:

	Application();
	~Application();

	bool Init();
	update_status Update(float dt);
	bool CleanUp();

    ModuleOpenGL* GetOpenGL() { return render; }
    ModuleWindow* GetWindow() { return window; }
    ModuleInput*  GetInput() { return input; }
    ModuleEditor*  GetEditor() { return editor; }
    ModuleCamera* GetCamera() { return camera;  }
    ModuleDebugDraw* GetDebugDraw() { return debugDraw; }
    ModuleFileSystem* GetFileSystem() { return fileSystem; }
    ModuleScene* GetScene() { return scene; }
    ModuleUI* GetUI() { return ui; }

    Timer* GetEngineClock() const { return mEngineTimer; }
    Timer* GetGameClock() const { return mGameTimer; }
    Timer* GetCurrentClock() const { return mCurrentTimer; }
    void SetCurrentClock(Timer* clock) { mCurrentTimer = clock; }

    //In case we want to use precise timer

    //PreciseTimer* GetEngineClock() const { return mEngineTimer; }
    //PreciseTimer* GetGameClock() const { return mGameTimer; }
    //PreciseTimer* GetCurrentClock() const { return mCurrentTimer; }
    //void SetCurrentClock(PreciseTimer* clock) { mCurrentTimer = clock; }

    float GetRealDt() const;
    float GetGameDt() const;

private:

    ModuleOpenGL* render = nullptr;
    ModuleWindow* window = nullptr;
    ModuleInput* input = nullptr;
    ModuleEditor* editor = nullptr;
    ModuleCamera* camera = nullptr;
    ModuleDebugDraw* debugDraw = nullptr;
    ModuleFileSystem* fileSystem = nullptr;
    ModuleScene* scene = nullptr;
    ModuleUI* ui = nullptr;

#define NUM_MODULES 9
    Module* modules[NUM_MODULES];

    //Timer
    Timer* mEngineTimer;
    Timer* mGameTimer;
    Timer* mCurrentTimer = nullptr;

    //In case we want to use precise timer
    
    //PreciseTimer* mEngineTimer;
    //PreciseTimer* mGameTimer;
    //PreciseTimer* mCurrentTimer = nullptr;

    bool mEnableVsync = true;
};

extern Application* App;
