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
	float2 GetGameWindowsPosition() const { return mGameWindowsPosition; }
	void SetGameWindowsPosition(float2 gameWindowsPosition) { mGameWindowsPosition = gameWindowsPosition; }
	void SetMousePositionInWindow(float2 position);

	void GameWindowsResized(float2 gameWindowsSize) { mGameWindowsSize = gameWindowsSize; }
	void WindowResized(unsigned width, unsigned height);
	void WindowFullscreen(bool value);

	SDL_Window* mWindow = nullptr;
	SDL_Surface* mScreenSurface = nullptr;

private:
	int width = 0;
	int height = 0;

	float2 mGameWindowsSize;
	float2 mGameWindowsPosition;
};

#endif /* _MODULE_WINDOW_H_ */