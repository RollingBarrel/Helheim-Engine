#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "SDL.h"
#include "imgui_impl_sdl2.h"
#include "ModuleFileSystem.h"
#include "ModuleResource.h"
//#include "SDL_scancode.h"

ModuleInput::ModuleInput()
{
	keyboard = new KeyState[SDL_NUM_SCANCODES];
	memset(keyboard, 0, SDL_NUM_SCANCODES);

    gameController.mButtons = new ButtonState[NUM_CONTROLLER_BUTTONS];
    memset(gameController.mButtons, 0, sizeof(ButtonState) * NUM_CONTROLLER_BUTTONS);		// 0 = BUTTON_IDLE

    gameController.mTriggers = new ButtonState[NUM_CONTROLLER_TRIGGERS];						// Only 2 triggers are supported for a given game controller.
    memset(gameController.mTriggers, 0, sizeof(ButtonState) * NUM_CONTROLLER_TRIGGERS);		// 0 = BUTTON_IDLE

    gameController.mAxis = new AxisState[NUM_CONTROLLER_AXIS];								// Only 4 axis are supported for a given game controller.
    memset(gameController.mAxis, 0, sizeof(AxisState) * NUM_CONTROLLER_AXIS);				// 0 = AXIS_IDLE
}

// Destructor
ModuleInput::~ModuleInput()
{
	delete[] keyboard;
    delete gameController.mButtons;
    delete gameController.mTriggers;
    delete gameController.mAxis;
}

bool ModuleInput::Init()
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);


    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0)										
    {
        LOG("SDL_GAMECONTROLLER could not initialize! SDL_Error: %s\n", SDL_GetError());
        ret = false;
    }

    SDL_GameControllerEventState(SDL_ENABLE);													

    if (SDL_GameControllerEventState(SDL_QUERY) != SDL_ENABLE)									//Checking the state of the game controller event. QUERY will return 1 on ENABLE and 0 on IGNORE.
    {
        LOG("The Controller Event State could not be enabled! SDL_Error: %s\n", SDL_GetError());
        ret = false;		
    }

	if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}


// Called each loop iteration
update_status ModuleInput::PreUpdate(float dt)
{
    //TODO: ugly reset !!
    if (wheelY != 0)
    {
        wheelY = 0;
    }

    //MOUSE

    mMouseReceivedInput = false;
    unsigned int mouseBitmask = SDL_GetRelativeMouseState(&mMouseMotionX, &mMouseMotionY);
    SDL_GetMouseState(&mMousePositionX, &mMousePositionY);
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

            mMouseReceivedInput = true;
        }
        else
        {
            switch (mouse[i])
            {
            case KeyState::KEY_DOWN:
            case KeyState::KEY_REPEAT:
                mouse[i] = KeyState::KEY_UP;
                mMouseReceivedInput = true;
                break;
            default:
                mouse[i] = KeyState::KEY_IDLE;
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
            mKeyboardReceivedInput = true;
        }
        else
        {
            switch (keyboard[i])
            {
            case KeyState::KEY_REPEAT:
            case KeyState::KEY_DOWN:
                keyboard[i] = KeyState::KEY_UP;
				mKeyboardReceivedInput = true;
                break;
            default:
                keyboard[i] = KeyState::KEY_IDLE;
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
		case SDL_CONTROLLERDEVICEADDED:
			if (gameController.mId == nullptr)
			{
				if (SDL_NumJoysticks() > 0)
				{
					if (SDL_IsGameController(CONTROLLER_INDEX))
					{
						gameController.mId = SDL_GameControllerOpen(CONTROLLER_INDEX);
						gameController.mJoystick = SDL_GameControllerGetJoystick(gameController.mId);
					}
				}
				else
				{
					LOG("No joysticks currently attached to the system. SDL_Error: %s\n", SDL_GetError());
				}
			}
        case SDL_DROPFILE:
            LOG("File droped: %s\n", sdlEvent.drop.file);
            App->GetFileSystem()->NormalizePath(sdlEvent.drop.file);
            App->GetResource()->ImportFile(sdlEvent.drop.file);

            App->GetFileSystem()->GetRootNode()->mChildren.clear();
            App->GetFileSystem()->DiscoverFiles("Assets", App->GetFileSystem()->GetRootNode());
            SDL_free(sdlEvent.drop.file);
            break;
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

int ModuleInput::GetGameControllerAxisValue(int id) const
{
	if (gameController.mId != nullptr)
	{
		if (id < 2)
		{
			if (gameController.mAxis[id] != AxisState::AXIS_IDLE)
				return SDL_GameControllerGetAxis(gameController.mId, SDL_GameControllerAxis(id));
		}
		else if (id > 1)
		{
			if (SDL_GameControllerGetAxis(gameController.mId, SDL_GameControllerAxis(id)) > JOYSTICK_THRESHOLD ||
				SDL_GameControllerGetAxis(gameController.mId, SDL_GameControllerAxis(id)) < -JOYSTICK_THRESHOLD)
				return SDL_GameControllerGetAxis(gameController.mId, SDL_GameControllerAxis(id));
		}

	}

	return 0;
}

int ModuleInput::GetGameControllerAxisRaw(int id) const
{
	if (gameController.mId != nullptr)
	{
		if (id < 2)
		{
			if (gameController.mAxis[id] != AxisState::AXIS_IDLE)
				return SDL_GameControllerGetAxis(gameController.mId, SDL_GameControllerAxis(id));
		}
		else if (id > 1)
		{
			return SDL_GameControllerGetAxis(gameController.mId, SDL_GameControllerAxis(id));
		}
	}

	return 0;
}

int ModuleInput::GetGameControllerAxisInput(int id) const
{
	return ((gameController.mId != nullptr) ? SDL_GameControllerGetAxis(gameController.mId, SDL_GameControllerAxis(id)) : 0);
}

void ModuleInput::HandleGameControllerInput()
{
	mGameControllerReceivedInput = false;

	if (!SDL_GameControllerGetAttached(gameController.mId))													
	{
		if (gameController.mId != nullptr)
		{
			SDL_GameControllerClose(gameController.mId);
			gameController.mId = nullptr;
		}
		return;
	}

	for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i)														
	{
		if (SDL_GameControllerGetButton(gameController.mId, SDL_GameControllerButton(i)) == SDL_PRESSED)		// Gets the current state of a button in a controller. 1 = PRESSED / 0 = RELEASED.
		{
			switch (gameController.mButtons[i])
			{
			case ButtonState::BUTTON_IDLE:
			case ButtonState::BUTTON_UP:
				gameController.mButtons[i] = ButtonState::BUTTON_DOWN;
				break;
			default:
				gameController.mButtons[i] = ButtonState::BUTTON_REPEAT;
				break;
			}
			mGameControllerReceivedInput = true;
		}
		else
		{
			switch (gameController.mButtons[i])
			{
			case ButtonState::BUTTON_REPEAT:
			case ButtonState::BUTTON_DOWN:
				gameController.mButtons[i] = ButtonState::BUTTON_UP;
				mGameControllerReceivedInput = true;
				break;
			default:
				gameController.mButtons[i] = ButtonState::BUTTON_IDLE;
				break;
			}
		}
	}

	for (int i = 0; i < NUM_CONTROLLER_TRIGGERS; ++i)
	{
		int trigger_value = SDL_GameControllerGetAxis(gameController.mId, SDL_GameControllerAxis(TRIGGER_INDEX + i));
	
		if (trigger_value > gameController.mMaxAxisInputThreshold * MAX_AXIS)
		{
			switch (gameController.mTriggers[i])
			{
			case ButtonState::BUTTON_IDLE:
			case ButtonState::BUTTON_UP:
				gameController.mTriggers[i] = ButtonState::BUTTON_DOWN;
				break;
			default:
				gameController.mTriggers[i] = ButtonState::BUTTON_REPEAT;
				break;
			}
			mGameControllerReceivedInput = true;
		}
		else
		{
			switch (gameController.mTriggers[i])
			{
			case ButtonState::BUTTON_REPEAT:
			case ButtonState::BUTTON_DOWN:
				gameController.mTriggers[i] = ButtonState::BUTTON_UP;
				mGameControllerReceivedInput = true;
				break;
			default:
				gameController.mTriggers[i] = ButtonState::BUTTON_IDLE;
				break;
			}
		}
	}

	for (int i = 0; i < NUM_CONTROLLER_AXIS; ++i)
	{
		int axis_value = SDL_GameControllerGetAxis(gameController.mId, SDL_GameControllerAxis(i));			

		if (abs(axis_value) < (int)(gameController.mMinAxisInputThreshold * MAX_AXIS))
		{
			axis_value = 0;																					
		}
		else
		{
			mGameControllerReceivedInput = true;
		}


		if (axis_value > gameController.mMaxPositiveThreshold)												
		{
			switch (gameController.mAxis[i])
			{
			case AxisState::AXIS_IDLE:
				gameController.mAxis[i] = AxisState::POSITIVE_AXIS_DOWN;
				break;
			default:
				gameController.mAxis[i] = AxisState::POSITIVE_AXIS_REPEAT;
				break;
			}
			mGameControllerReceivedInput = true;
		}
		else
		{
			if (axis_value < gameController.mMinPositiveThreshold && axis_value > gameController.mMinNegativeThreshold)	
			{
				switch (gameController.mAxis[i])
				{
				case AxisState::POSITIVE_AXIS_DOWN:
				case AxisState::POSITIVE_AXIS_REPEAT:
					gameController.mAxis[i] = AxisState::POSITIVE_AXIS_RELEASE;
					mGameControllerReceivedInput = true;
					break;
				default:
					gameController.mAxis[i] = AxisState::AXIS_IDLE;
					break;
				}
			}
		}

		if (axis_value < gameController.mMaxNegativeThreshold)																
		{
			switch (gameController.mAxis[i])
			{
			case AxisState::AXIS_IDLE:
				gameController.mAxis[i] = AxisState::NEGATIVE_AXIS_DOWN;
				break;
			default:
				gameController.mAxis[i] = AxisState::NEGATIVE_AXIS_REPEAT;
				break;
			}
			mGameControllerReceivedInput = true;
		}
		else if (axis_value > gameController.mMinNegativeThreshold && axis_value < gameController.mMinPositiveThreshold)	
		{
			switch (gameController.mAxis[i])
			{
			case AxisState::NEGATIVE_AXIS_DOWN:
			case AxisState::NEGATIVE_AXIS_REPEAT:
				gameController.mAxis[i] = AxisState::NEGATIVE_AXIS_RELEASE;
				mGameControllerReceivedInput = true;
				break;
			default:
				gameController.mAxis[i] = AxisState::AXIS_IDLE;
				break;
			}
		}
	}
}
