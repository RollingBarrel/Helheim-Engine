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
class ModuleScriptManager;
class ModuleResource;
class ModuleUI;
class ModuleDetourNavigation;
class ModuleAudio;
class ModuleEvent;
class ModulePrograms;

class Timer;
class PreciseTimer;

class ENGINE_API Application
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
    ModuleScriptManager* GetScriptManager() { return scriptManager; }
    ModuleUI* GetUI() { return ui; }
    ModuleResource* GetResource() { return resource; }
    ModuleDetourNavigation* GetNavigation() { return navigation; }
    ModuleAudio* GeAudio() { return audio; }
    ModuleEvent* GetEvent() { return event; }

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
    ModuleScriptManager* scriptManager = nullptr;
    ModuleResource* resource = nullptr;
    ModuleAudio* audio = nullptr;
    ModuleUI* ui = nullptr;
    ModuleDetourNavigation* navigation = nullptr;
    ModuleEvent* event = nullptr;

#define NUM_MODULES 14
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

 extern ENGINE_API Application* App;
