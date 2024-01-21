#ifndef _MODULE_WINDOW_H_
#define _MODULE_WINDOW_H_

#include "Module.h"

struct SDL_Window;
struct SDL_Surface;

class ModuleWindow : public Module
{
public:

	ModuleWindow();

	// Destructor
	~ModuleWindow();

	bool Init() override;
	bool CleanUp() override;


	int GetWidth() const { return width; };
	int GetHeight() const { return height; };
	float GetAspectRatio() const { return (float)width / (float)height; };
	void WindowResized(unsigned width, unsigned height);

	SDL_Window* window = nullptr;
	SDL_Surface* screen_surface = nullptr;

private:
	int width = 0;
	int height = 0;
};

#endif /* _MODULE_WINDOW_H_ */