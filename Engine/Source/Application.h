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
class ModuleCamera;
class ModuleRenderTest;
class ModuleDebugDraw;
class ModuleFileSystem;
class ModuleScene;

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
    ModuleRenderTest* GetTest() { return test; }
    ModuleDebugDraw* GetDebugDraw() { return debugDraw; }
    ModuleFileSystem* GetFileSystem() { return fileSystem; }
    ModuleScene* GetScene() { return scene; }

private:

    ModuleOpenGL* render = nullptr;
    ModuleWindow* window = nullptr;
    ModuleInput* input = nullptr;
    ModuleEditor* editor = nullptr;
    ModuleCamera* camera = nullptr;
    ModuleRenderTest* test = nullptr;
    ModuleDebugDraw* debugDraw = nullptr;
    ModuleFileSystem* fileSystem = nullptr;
    ModuleScene* scene = nullptr;

    std::list<Module*> modules;

};

extern Application* App;
