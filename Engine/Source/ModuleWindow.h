#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow();

	// Destructor
	~ModuleWindow();

	bool Init() override;
	bool CleanUp() override;

public:

	SDL_Window* window = nullptr;
	SDL_Surface* screen_surface = nullptr;
};

#endif // __ModuleWindow_H__