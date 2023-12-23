#pragma once
#include <list>
#include "Globals.h"
#include "Module.h"

class ModuleOpenGL;
class ModuleWindow;
class ModuleInput;
class ModuleEditor;
class ModuleProgram;

class Application
{
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

    ModuleOpenGL* GetOpenGL() { return mRender; }
    ModuleWindow* GetWindow() { return mWindow; }
    ModuleInput*  GetInput() { return mInput; }
    ModuleEditor*  GetEditor() { return mEditor; }
    ModuleProgram* GetProgram() { return mProgram; }


private:

    ModuleOpenGL* mRender = nullptr;
    ModuleWindow* mWindow = nullptr;
    ModuleInput* mInput = nullptr;
    ModuleEditor* mEditor = nullptr;
    ModuleProgram* mProgram = nullptr;

    std::list<Module*> mModules;

};

extern Application* App;
