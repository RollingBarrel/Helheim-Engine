#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "SDL.h"
#include "ModuleFileSystem.h"
#include "ModuleResource.h"
//#include "SDL_scancode.h"

ModuleInput::ModuleInput()
{
	

	mKeyboard = new KeyState[SDL_NUM_SCANCODES];
	memset(mKeyboard, 0, SDL_NUM_SCANCODES);

    mGameController.mButtons = new ButtonState[SDL_CONTROLLER_BUTTON_MAX];
    memset(mGameController.mButtons, 0, sizeof(ButtonState) * SDL_CONTROLLER_BUTTON_MAX);		// 0 = BUTTON_IDLE

	mGameController.mTriggers = new ButtonState[NUM_CONTROLLER_TRIGGERS];						// Only 2 triggers are supported for a given game controller.
    memset(mGameController.mTriggers, 0, sizeof(ButtonState) * NUM_CONTROLLER_TRIGGERS);		// 0 = BUTTON_IDLE

	mGameController.mAxis = new AxisState[NUM_CONTROLLER_AXIS];								// Only 4 axis are supported for a given game controller.
    memset(mGameController.mAxis, 0, sizeof(AxisState) * NUM_CONTROLLER_AXIS);				// 0 = AXIS_IDLE
}

// Destructor
ModuleInput::~ModuleInput()
{
	delete[] mKeyboard;
    delete[] mGameController.mButtons;
    delete[] mGameController.mTriggers;
    delete[] mGameController.mAxis;
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

float ModuleInput::GetGameControllerAxisValue(int id) const
{
	if (mGameController.mId != nullptr)
	{
		if (/*mGameController.mAxis[id] != AxisState::AXIS_IDLE &&*/
			SDL_GameControllerGetAxis(mGameController.mId, SDL_GameControllerAxis(id)) > JOYSTICK_THRESHOLD ||
			SDL_GameControllerGetAxis(mGameController.mId, SDL_GameControllerAxis(id)) < -JOYSTICK_THRESHOLD)
		{
			LOG("%d: %f", id, SDL_GameControllerGetAxis(mGameController.mId, SDL_GameControllerAxis(id)) / static_cast<float>(SDL_JOYSTICK_AXIS_MAX));
			return SDL_GameControllerGetAxis(mGameController.mId, SDL_GameControllerAxis(id)) / static_cast<float>(SDL_JOYSTICK_AXIS_MAX);
		}
		

	}

	return 0.0f;
}

int ModuleInput::GetGameControllerAxisRaw(int id) const
{
	if (mGameController.mId != nullptr)
	{
		if (id < 2)
		{
			if (mGameController.mAxis[id] != AxisState::AXIS_IDLE)
				return SDL_GameControllerGetAxis(mGameController.mId, SDL_GameControllerAxis(id));
		}
		else if (id > 1)
		{
			return SDL_GameControllerGetAxis(mGameController.mId, SDL_GameControllerAxis(id));
		}
	}

	return 0;
}

int ModuleInput::GetGameControllerAxisInput(int id) const
{
	return ((mGameController.mId != nullptr) ? SDL_GameControllerGetAxis(mGameController.mId, SDL_GameControllerAxis(id)) : 0);
}

void ModuleInput::HandleGameControllerInput()
{
	mGameControllerReceivedInput = false;

	if (!SDL_GameControllerGetAttached(mGameController.mId))
	{
		if (mGameController.mId != nullptr)
		{
			SDL_GameControllerClose(mGameController.mId);
			mGameController.mId = nullptr;
		}
		return;
	}

	for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i)														
	{
		if (SDL_GameControllerGetButton(mGameController.mId, SDL_GameControllerButton(i)) == SDL_PRESSED)		// Gets the current state of a button in a controller. 1 = PRESSED / 0 = RELEASED.
		{
			switch (mGameController.mButtons[i])
			{
			case ButtonState::BUTTON_IDLE:
			case ButtonState::BUTTON_UP:
				mGameController.mButtons[i] = ButtonState::BUTTON_DOWN;
				break;
			default:
				mGameController.mButtons[i] = ButtonState::BUTTON_REPEAT;
				break;
			}
			mGameControllerReceivedInput = true;
		}
		else
		{
			switch (mGameController.mButtons[i])
			{
			case ButtonState::BUTTON_REPEAT:
			case ButtonState::BUTTON_DOWN:
				mGameController.mButtons[i] = ButtonState::BUTTON_UP;
				mGameControllerReceivedInput = true;
				break;
			default:
				mGameController.mButtons[i] = ButtonState::BUTTON_IDLE;
				break;
			}
		}
	}

	for (int i = 0; i < NUM_CONTROLLER_TRIGGERS; ++i)
	{
		int trigger_value = SDL_GameControllerGetAxis(mGameController.mId, SDL_GameControllerAxis(TRIGGER_INDEX + i));
	
		if (trigger_value > mGameController.mMaxAxisInputThreshold * MAX_AXIS)
		{
			switch (mGameController.mTriggers[i])
			{
			case ButtonState::BUTTON_IDLE:
			case ButtonState::BUTTON_UP:
				mGameController.mTriggers[i] = ButtonState::BUTTON_DOWN;
				break;
			default:
				mGameController.mTriggers[i] = ButtonState::BUTTON_REPEAT;
				break;
			}
			mGameControllerReceivedInput = true;
		}
		else
		{
			switch (mGameController.mTriggers[i])
			{
			case ButtonState::BUTTON_REPEAT:
			case ButtonState::BUTTON_DOWN:
				mGameController.mTriggers[i] = ButtonState::BUTTON_UP;
				mGameControllerReceivedInput = true;
				break;
			default:
				mGameController.mTriggers[i] = ButtonState::BUTTON_IDLE;
				break;
			}
		}
	}

	for (int i = 0; i < NUM_CONTROLLER_AXIS; ++i)
	{
		int axis_value = SDL_GameControllerGetAxis(mGameController.mId, SDL_GameControllerAxis(i));

		if (abs(axis_value) < (int)(mGameController.mMinAxisInputThreshold * MAX_AXIS))
		{
			axis_value = 0;																					
		}
		else
		{
			mGameControllerReceivedInput = true;
		}


		if (axis_value > mGameController.mMaxPositiveThreshold)
		{
			switch (mGameController.mAxis[i])
			{
			case AxisState::AXIS_IDLE:
				mGameController.mAxis[i] = AxisState::POSITIVE_AXIS_DOWN;
				break;
			default:
				mGameController.mAxis[i] = AxisState::POSITIVE_AXIS_REPEAT;
				break;
			}
			mGameControllerReceivedInput = true;
		}
		else
		{
			if (axis_value < mGameController.mMinPositiveThreshold && axis_value > mGameController.mMinNegativeThreshold)
			{
				switch (mGameController.mAxis[i])
				{
				case AxisState::POSITIVE_AXIS_DOWN:
				case AxisState::POSITIVE_AXIS_REPEAT:
					mGameController.mAxis[i] = AxisState::POSITIVE_AXIS_RELEASE;
					mGameControllerReceivedInput = true;
					break;
				default:
					mGameController.mAxis[i] = AxisState::AXIS_IDLE;
					break;
				}
			}
		}

		if (axis_value < mGameController.mMaxNegativeThreshold)
		{
			switch (mGameController.mAxis[i])
			{
			case AxisState::AXIS_IDLE:
				mGameController.mAxis[i] = AxisState::NEGATIVE_AXIS_DOWN;
				break;
			default:
				mGameController.mAxis[i] = AxisState::NEGATIVE_AXIS_REPEAT;
				break;
			}
			mGameControllerReceivedInput = true;
		}
		else if (axis_value > mGameController.mMinNegativeThreshold && axis_value < mGameController.mMinPositiveThreshold)
		{
			switch (mGameController.mAxis[i])
			{
			case AxisState::NEGATIVE_AXIS_DOWN:
			case AxisState::NEGATIVE_AXIS_REPEAT:
				mGameController.mAxis[i] = AxisState::NEGATIVE_AXIS_RELEASE;
				mGameControllerReceivedInput = true;
				break;
			default:
				mGameController.mAxis[i] = AxisState::AXIS_IDLE;
				break;
			}
		}
	}
}
