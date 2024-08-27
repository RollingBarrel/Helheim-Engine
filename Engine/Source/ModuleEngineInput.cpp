#include "Globals.h"
#include "EngineApp.h"
#include "ModuleEngineInput.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "SDL.h"
#include "imgui_impl_sdl2.h"
#include "ModuleFileSystem.h"
#include "ModuleEngineResource.h"

ModuleEngineInput::ModuleEngineInput() : ModuleInput()
{
}

// Destructor
ModuleEngineInput::~ModuleEngineInput()
{
}


// Called each loop iteration
update_status ModuleEngineInput::PreUpdate(float dt)
{
	//TODO: ugly reset !!
	if (mWheelY != 0)
	{
		mWheelY = 0;
	}

	//MOUSE

	mMouseReceivedInput = false;
	unsigned int mouseBitmask = SDL_GetRelativeMouseState(&mMouseMotionX, &mMouseMotionY);
	SDL_GetMouseState(&mMouseLocalPositionX, &mMouseLocalPositionY);
	SDL_GetGlobalMouseState(&mMouseGlobalPositionX, &mMouseGlobalPositionY);
	for (int i = 0; i < MouseKey::NUM_MOUSE_BUTTONS; ++i)
	{
		unsigned int pressed = mouseBitmask & SDL_BUTTON(i + 1);
		if (pressed)
		{
			switch (mMouse[i])
			{
			case KeyState::KEY_IDLE:
			case KeyState::KEY_UP:
				mMouse[i] = KeyState::KEY_DOWN;
				break;
			default:
				mMouse[i] = KeyState::KEY_REPEAT;
				break;
			}

			mMouseReceivedInput = true;
		}
		else
		{
			switch (mMouse[i])
			{
			case KeyState::KEY_DOWN:
			case KeyState::KEY_REPEAT:
				mMouse[i] = KeyState::KEY_UP;
				mMouseReceivedInput = true;
				break;
			default:
				mMouse[i] = KeyState::KEY_IDLE;
				break;
			}
		}
	}

	//KEYBOARD
	mKeyboardReceivedInput = false;
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	for (int i = 0; i < KeyboardKeys::KeyboardKeys_NUM_SCANCODES; ++i)
	{
		if (keys[i] == SDL_PRESSED)
		{
			switch (mKeyboard[i])
			{
			case KeyState::KEY_IDLE:
			case KeyState::KEY_UP:
				mKeyboard[i] = KeyState::KEY_DOWN;
				break;
			default:
				mKeyboard[i] = KeyState::KEY_REPEAT;
				break;
			}
			mKeyboardReceivedInput = true;
		}
		else
		{
			switch (mKeyboard[i])
			{
			case KeyState::KEY_REPEAT:
			case KeyState::KEY_DOWN:
				mKeyboard[i] = KeyState::KEY_UP;
				mKeyboardReceivedInput = true;
				break;
			default:
				mKeyboard[i] = KeyState::KEY_IDLE;
				break;
			}
		}
	}

	//GAME CONTROLLER
	HandleGameControllerInput();


	SDL_Event sdlEvent;

	while (SDL_PollEvent(&sdlEvent) != 0)
	{
		ImGui_ImplSDL2_ProcessEvent(&sdlEvent);

		switch (sdlEvent.type)
		{
		case SDL_QUIT:
			return UPDATE_STOP;
		case SDL_WINDOWEVENT:
			//https://github.com/ocornut/imgui/blob/master/examples/example_sdl2_opengl3/main.cpp
			//if (sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED || sdlEvent.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
			if (sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				EngineApp->GetWindow()->WindowResized(sdlEvent.window.data1, sdlEvent.window.data2);
				EngineApp->GetOpenGL()->WindowResized(sdlEvent.window.data1, sdlEvent.window.data2);
			}
			if (sdlEvent.window.event == SDL_WINDOWEVENT_CLOSE)
			{
				return UPDATE_STOP;
			}
			break;
		case SDL_MOUSEWHEEL:
			mWheelY = sdlEvent.wheel.y;
			break;
		case SDL_CONTROLLERDEVICEADDED:
			if (mGameController.mId == nullptr)
			{
				if (SDL_NumJoysticks() > 0)
				{
					if (SDL_IsGameController(CONTROLLER_INDEX))
					{
						mGameController.mId = SDL_GameControllerOpen(CONTROLLER_INDEX);
						mGameController.mJoystick = SDL_GameControllerGetJoystick(mGameController.mId);
					}
				}
				else
				{
					LOG("No joysticks currently attached to the system. SDL_Error: %s\n", SDL_GetError());
				}
			}
			break;
		case SDL_DROPFILE:
			LOG("File droped: %s\n", sdlEvent.drop.file);
			EngineApp->GetFileSystem()->NormalizePath(sdlEvent.drop.file);
			EngineApp->GetEngineResource()->ImportFile(sdlEvent.drop.file);

			EngineApp->GetFileSystem()->GetRootNode()->CleanUp();
			EngineApp->GetFileSystem()->DiscoverFiles("Assets", EngineApp->GetFileSystem()->GetRootNode());
			SDL_free(sdlEvent.drop.file);
			break;
		}
	}

	return UPDATE_CONTINUE;
}