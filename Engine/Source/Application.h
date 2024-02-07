#pragma once
#include "Globals.h"
#include "Module.h"
#include "EngineTimer.h"

class ModuleOpenGL;
class ModuleWindow;
class ModuleInput;
class ModuleEditor;
class ModuleRenderExercise;
class ModuleScene;
class ModuleFileSystem;
class ModuleCamera;
class ModuleDebugDraw;
class ModuleTimer;

class Timer;

class Application
{
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

    ModuleOpenGL* GetOpenGL() { return render; }
    ModuleWindow* GetWindow() { return window; }
    ModuleInput*  GetInput() { return input; }
    ModuleEditor*  GetEditor() { return editor; }
    ModuleCamera* GetCamera() { return camera;  }
    ModuleDebugDraw* GetDebugDraw() { return debugDraw; }
    ModuleFileSystem* GetFileSystem() { return fileSystem; }
    ModuleScene* GetScene() { return scene; }
    ModuleTimer* GetClock() { return clock; }

    float GetDt() const;

private:

    ModuleOpenGL* render = nullptr;
    ModuleWindow* window = nullptr;
    ModuleInput* input = nullptr;
    ModuleEditor* editor = nullptr;
    ModuleCamera* camera = nullptr;
    ModuleDebugDraw* debugDraw = nullptr;
    ModuleFileSystem* fileSystem = nullptr;
    ModuleScene* scene = nullptr;
    ModuleTimer* clock = nullptr;

#define NUM_MODULES 9
    Module* modules[NUM_MODULES];


    //ModuleTimer stuff
    Timer* mRealTimer;
    Timer* mGameTimer;

    long mGameDelta = 0;
    unsigned long mDeltaTime = 0;
    long mUpdateTime = 0;
    bool mChangeSpeed = false;
    float mNewSpeed = 1;
    unsigned int mFpsLimit = 60;
    //std::vector<float> mFpsLog;
    bool mUpdateFpsLog = false;
};

extern Application* App;
