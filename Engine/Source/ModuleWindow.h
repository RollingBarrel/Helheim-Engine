#ifndef _MODULE_WINDOW_H_
#define _MODULE_WINDOW_H_

#include "Module.h"
#include "float2.h"

struct SDL_Window;
struct SDL_Surface;

class ENGINE_API ModuleWindow : public Module
{
public:

	ModuleWindow();

	// Destructor
	~ModuleWindow();

	bool Init() override;
	bool CleanUp() override;


	int GetWidth() const { return width; };
	int GetHeight() const { return height; };
	float GetAspectRatio() const { return (float)width / (float)height; }
	float2 GetGameWindowsSize() const { return mGameWindowsSize; }
	void GameWindowsResized(float2 gameWindowsSize) { mGameWindowsSize = gameWindowsSize; }
	float2 GetGameWindowsPosition() const { return mGameWindowsPosition; }
	void SetGameWindowsPosition(float2 gameWindowsPosition) { mGameWindowsPosition = gameWindowsPosition; }


	void WindowResized(unsigned width, unsigned height);

	SDL_Window* window = nullptr;
	SDL_Surface* screen_surface = nullptr;

private:
	int width = 0;
	int height = 0;

	float2 mGameWindowsSize;
	float2 mGameWindowsPosition;
};

#endif /* _MODULE_WINDOW_H_ */