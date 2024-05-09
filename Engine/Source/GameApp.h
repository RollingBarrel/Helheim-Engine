#include "Application.h"

class ModuleGame;

class GameApplication : public Application
{
public:
	GameApplication();
	~GameApplication();

	bool Init();
	update_status Update(float dt);
	bool CleanUp();
	ModuleGame* GetGame() { return game; }

private:
	ModuleGame* game = nullptr;

#define NUM_MODULES 13
	Module* modules[NUM_MODULES];
};

extern GameApplication* GameApp;