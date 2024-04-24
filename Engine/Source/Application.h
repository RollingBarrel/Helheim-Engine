#pragma once
#include "Globals.h"
#include "Module.h"

class ModuleOpenGL;
class ModuleWindow;
class ModuleInput;
class ModuleRenderExercise;
class ModuleScene;
class ModuleFileSystem;
class ModuleCamera;
class ModuleScriptManager;
class ModuleResource;
class ModuleUI;
class ModuleDetourNavigation;
class ModuleAudio;
class ModuleEvent;
#ifdef ENGINE
class ModuleEditor;
class ModuleDebugDraw;
#endif // ENGINE

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
    ModuleCamera* GetCamera() { return camera;  }
    ModuleFileSystem* GetFileSystem() { return fileSystem; }
    ModuleScene* GetScene() { return scene; }
    ModuleScriptManager* GetScriptManager() { return scriptManager; }
    ModuleUI* GetUI() { return ui; }
    ModuleResource* GetResource() { return resource; }
    ModuleDetourNavigation* GetNavigation() { return navigation; }
    ModuleAudio* GetAudio() { return audio; }
    ModuleEvent* GetEvent() { return event; }
#ifdef ENGINE
    ModuleEditor* GetEditor() { return editor; }
    ModuleDebugDraw* GetDebugDraw() { return debugDraw; }
#endif // ENGINE

    bool IsPlayMode() { return mIsPlayMode; }
    void PlayMode(bool play) { mIsPlayMode = play; }
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
    ModuleFileSystem* fileSystem = nullptr;
    ModuleCamera* camera = nullptr;
    ModuleScene* scene = nullptr;
    ModuleScriptManager* scriptManager = nullptr;
    ModuleResource* resource = nullptr;
    ModuleAudio* audio = nullptr;
    ModuleUI* ui = nullptr;
    ModuleDetourNavigation* navigation = nullptr;
    ModuleEvent* event = nullptr;
#ifdef ENGINE
    ModuleEditor* editor = nullptr;
    ModuleDebugDraw* debugDraw = nullptr;
#endif // ENGINE

#ifdef ENGINE
#define NUM_MODULES 14
#else
#define NUM_MODULES 12
#endif // ENGINE
    Module* modules[NUM_MODULES];

    //Timer
    Timer* mEngineTimer;
    Timer* mGameTimer;
    Timer* mCurrentTimer = nullptr;

    //In case we want to use precise timer
    
    //PreciseTimer* mEngineTimer;
    //PreciseTimer* mGameTimer;
    //PreciseTimer* mCurrentTimer = nullptr;

    bool mIsPlayMode = false;
    bool mEnableVsync = true;
};

 extern ENGINE_API Application* App;
