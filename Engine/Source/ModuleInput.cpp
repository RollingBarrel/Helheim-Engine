#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "SDL.h"
#include "imgui_impl_sdl2.h"

ModuleInput::ModuleInput()
{
	keyboard = new KeyState[SDL_NUM_SCANCODES];
	memset(keyboard, 0, SDL_NUM_SCANCODES);
}

// Destructor
ModuleInput::~ModuleInput()
{
	delete[] keyboard;
}

bool ModuleInput::Init()
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}


// Called each loop iteration
update_status ModuleInput::PreUpdate()
{
    //TODO: ugly reset !!
    if (wheelY != 0)
    {
        wheelY = 0;
    }

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
                App->GetWindow()->WindowResized(sdlEvent.window.data1, sdlEvent.window.data2);
                App->GetOpenGL()->WindowResized(sdlEvent.window.data1, sdlEvent.window.data2);
            }
            if (sdlEvent.window.event == SDL_WINDOWEVENT_CLOSE)
            {
                return UPDATE_STOP;
            }
            break;
        case SDL_MOUSEWHEEL:
            wheelY = sdlEvent.wheel.y;
            break;
        case SDL_DROPFILE:
            LOG("File droped: %s\n", sdlEvent.drop.file);
            SDL_free(sdlEvent.drop.file);
            break;
        }
    }

    //Mouse snapshot
    unsigned int mouseBitmask = SDL_GetRelativeMouseState(&mX, &mY);
    for (int i = 0; i < MouseKey::NUM_MOUSE_BUTTONS; ++i)
    {
        unsigned int pressed = mouseBitmask & SDL_BUTTON(i + 1);
        if (pressed)
        {
            switch (mouse[i])
            {
            case KeyState::KEY_IDLE:
            case KeyState::KEY_UP:
                mouse[i] = KeyState::KEY_DOWN;
                break;
            default:
                mouse[i] = KeyState::KEY_REPEAT;
                break;
            }
        }
        else
        {
            switch (mouse[i])
            {
            case KeyState::KEY_DOWN:
            case KeyState::KEY_REPEAT:
                mouse[i] = KeyState::KEY_UP;
                break;
            default:
                mouse[i] = KeyState::KEY_IDLE;
                break;
            }
        }
    }

    //keyboard snapshot
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i)
    {
        if (keys[i] == SDL_PRESSED)
        {
            switch (keyboard[i])
            {
            case KeyState::KEY_IDLE:
            case KeyState::KEY_UP:
                keyboard[i] = KeyState::KEY_DOWN;
                break;
            default:
                keyboard[i] = KeyState::KEY_REPEAT;
                break;
            }
        }
        else
        {
            switch (keyboard[i])
            {
            case KeyState::KEY_REPEAT:
            case KeyState::KEY_DOWN:
                keyboard[i] = KeyState::KEY_UP;
                break;
            default:
                keyboard[i] = KeyState::KEY_IDLE;
                break;
            }
        }
    }

    return UPDATE_CONTINUE;
}

bool ModuleInput::CleanUp()
{
    LOG("Quitting SDL input event subsystem");
    SDL_QuitSubSystem(SDL_INIT_EVENTS);
    return true;
}