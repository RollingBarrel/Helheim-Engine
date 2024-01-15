#pragma once
#include <list>
#include "Globals.h"
#include "Module.h"

class ModuleOpenGL;
class ModuleWindow;
class ModuleInput;
class ModuleEditor;
class ModuleRenderExercise;
class ModuleScene;
class ModuleTimer;
class ModuleCamera;
class ModuleDebugDraw;

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
    ModuleScene* GetScene() { return scene; }
    ModuleTimer* GetClock() { return clock; }
    ModuleCamera* GetCamera() { return camera;  }
    ModuleDebugDraw* GetDebugDraw() { return debugDraw; }


private:

    ModuleOpenGL* render = nullptr;
    ModuleWindow* window = nullptr;
    ModuleInput* input = nullptr;
    ModuleEditor* editor = nullptr;
    ModuleScene* scene = nullptr;
    ModuleTimer* clock = nullptr;
    ModuleCamera* camera = nullptr;
    ModuleDebugDraw* debugDraw = nullptr;


    std::list<Module*> mModules;

};

extern Application* App;
