#include "ModuleWindow.h"
#include "Globals.h"
#include "SDL.h"
#include "Application.h"
#include "ModuleResource.h"
#include "ResourceTexture.h"

#include "glew.h"

ModuleWindow::ModuleWindow()
{
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4); // desired version
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // we want a double buffer
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // we want to have a depth buffer with 24 bits
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); // we want to have a stencil buffer with 8 bits

#ifdef _DEBUG
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif // _DEBUG

		//Create window
		mWidth = SCREEN_WIDTH;
		mHeight = SCREEN_HEIGHT;
		Uint32 flags = SDL_WINDOW_SHOWN |  SDL_WINDOW_OPENGL;

		if (FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}
		if (RESIZEABLE == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		mWindow = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mWidth, mHeight, flags);

		if(mWindow == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			
			mScreenSurface = SDL_GetWindowSurface(mWindow);
		}
	}
	
	int windowPositionX, windowPositionY;
	SDL_GetWindowPosition(mWindow, &windowPositionX, &windowPositionY);
	mGameWindowsSize = float2(mWidth, mHeight);
	mGameWindowsPosition = float2(windowPositionX, windowPositionY);

	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(mWindow)
	{
		SDL_DestroyWindow(mWindow);
	}

	if (mCursor)
	{
		SDL_FreeCursor(mCursor);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

float2 ModuleWindow::GetScreenSize() const
{
	float2 size;
	SDL_DisplayMode displayMode;
	SDL_GetDesktopDisplayMode(SDL_GetWindowDisplayIndex(mWindow), &displayMode);
	size.x = displayMode.w;
	size.y = displayMode.h;

	return size;
}

void ModuleWindow::SetMousePositionInWindow(float2 position)
{
	SDL_WarpMouseInWindow(mWindow, position.x, position.y);
}

void ModuleWindow::WindowResized(unsigned width, unsigned height)
{
	this->mWidth = width;
	this->mHeight = height;
	mGameWindowsSize = float2(mWidth, mHeight);
}

void ModuleWindow::WindowMoved(unsigned x, unsigned y)
{
	mGameWindowsPosition.x = x;
	mGameWindowsPosition.y = y;
}

void ModuleWindow::WindowFullscreen(bool value)
{
	if (value)
	{
		SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
	else
	{
		SDL_SetWindowFullscreen(mWindow, 0);
	}

	int w, h;
	SDL_GetWindowSize(mWindow, &w, &h);
	WindowResized(w, h);
}

bool ModuleWindow::IsWindowFullscreen()
{
	Uint32 windowFlags = SDL_GetWindowFlags(mWindow);
	return (windowFlags & SDL_WINDOW_FULLSCREEN_DESKTOP) == SDL_WINDOW_FULLSCREEN_DESKTOP;
}

void ModuleWindow::SetCursor(unsigned int resourceID, unsigned int cursorWidth, unsigned int cursorHeight, unsigned int hotPointX, unsigned int hotPointY)
{
	if (mCursor)
	{
		SDL_FreeCursor(mCursor);
		mCursor = nullptr;
	}

	if (resourceID == 0)
	{
		SDL_SetCursor(SDL_GetDefaultCursor());
		return;
	}

	ResourceTexture* texture = static_cast<ResourceTexture*>(App->GetResource()->RequestResource(resourceID, Resource::Type::Texture));

	int w = 0;
	int h = 0;
	glGetTextureLevelParameteriv(texture->GetOpenGLId(), 0, GL_TEXTURE_WIDTH, &w);
	glGetTextureLevelParameteriv(texture->GetOpenGLId(), 0, GL_TEXTURE_HEIGHT, &h);
	void* pixel_data = malloc(w * h * 4);

	glBindTexture(GL_TEXTURE_2D, texture->GetOpenGLId());
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel_data);

	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(pixel_data, w, h, 32, 4 * w, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);

	// Scale image
	SDL_Surface* scaledImage = SDL_CreateRGBSurfaceWithFormat(0, cursorWidth, cursorHeight, 32, SDL_PIXELFORMAT_RGBA32);
	SDL_SoftStretch(surface, &surface->clip_rect, scaledImage, &scaledImage->clip_rect);

	if (scaledImage)
	{
		mCursor = SDL_CreateColorCursor(scaledImage, hotPointX, hotPointY);
		SDL_SetCursor(mCursor);
	}
	else
	{
		LOG("Error creating Cursor");
		SDL_SetCursor(SDL_GetDefaultCursor());
	}
	free(pixel_data);
	pixel_data = nullptr;
}



