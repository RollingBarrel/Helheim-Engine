#pragma once
#include <list>
#include "Globals.h"
#include "Module.h"

class ModuleOpenGL;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleEditor;
class ModuleRenderExercise;
class ModuleScene;
class ModuleTimer;
class ModuleCamera;
class ModuleDebugDraw;
class ModuleFileSystem;
class ModuleRenderTest;

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
    ModuleRenderTest* GetTest() { return test; }
    ModuleFileSystem* GetFileSystem() { return fileSystem; }


private:

    ModuleOpenGL* render = nullptr;
    ModuleWindow* window = nullptr;
    ModuleInput* input = nullptr;
    ModuleEditor* editor = nullptr;
    ModuleScene* scene = nullptr;
    ModuleTimer* clock = nullptr;
    ModuleCamera* camera = nullptr;
    ModuleDebugDraw* debugDraw = nullptr;
    ModuleRenderTest* test = nullptr;
    ModuleFileSystem* fileSystem = nullptr;

    std::list<Module*> modules;

};

extern Application* App;
