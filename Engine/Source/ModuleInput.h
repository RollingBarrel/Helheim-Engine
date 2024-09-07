#ifndef _MODULE_INPUT_H_
#define _MODULE_INPUT_H_

#include "Module.h"
#include "Globals.h"
#include "KeyboardKeys.h"
#include "float2.h"

#define NUM_CONTROLLER_BUTTONS 15
#define NUM_CONTROLLER_AXIS 4
#define NUM_CONTROLLER_TRIGGERS 2

#define MAX_AXIS 32767								
#define CONTROLLER_INDEX 0							
#define TRIGGER_INDEX 4		
#define JOYSTICK_THRESHOLD 2500

#define LEFT_TRIGGER 0							
#define RIGHT_TRIGGER 1

#define LEFT_JOYSTICK_X_AXIS 0
#define LEFT_JOYSTICK_Y_AXIS 1
#define RIGHT_JOYSTICK_X_AXIS 2
#define RIGHT_JOYSTICK_Y_AXIS 3

struct _SDL_GameController;
struct _SDL_Joystick;

enum class KeyState : unsigned char
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

enum MouseKey
{
	BUTTON_LEFT,
	BUTTON_MIDDLE,
	BUTTON_RIGHT,
	BUTTON_X1,
	BUTTON_X2,

	NUM_MOUSE_BUTTONS
};

enum class ButtonState
{
	BUTTON_IDLE,
	BUTTON_DOWN,
	BUTTON_REPEAT,
	BUTTON_UP,
	UNKNOWN_BUTTON
};

enum class AxisState
{
	AXIS_IDLE,
	POSITIVE_AXIS_DOWN,
	POSITIVE_AXIS_REPEAT,
	POSITIVE_AXIS_RELEASE,
	NEGATIVE_AXIS_DOWN,
	NEGATIVE_AXIS_REPEAT,
	NEGATIVE_AXIS_RELEASE,
	UNKNOWN_AXIS
};

struct GameController
{
	_SDL_GameController* mId = nullptr;
	_SDL_Joystick* mJoystick = nullptr;
	int	mIndex = CONTROLLER_INDEX;

	ButtonState* mButtons = 0;
	ButtonState* mTriggers;
	AxisState* mAxis = 0;

	float mMaxAxisInputThreshold = 1.0f;
	float mMinAxisInputThreshold = 0.1f;

	int	mMaxPositiveThreshold = (int)(mMaxAxisInputThreshold * MAX_AXIS);
	int	mMaxNegativeThreshold = -(int)(mMaxAxisInputThreshold * MAX_AXIS);
	int	mMinPositiveThreshold = (int)(mMinAxisInputThreshold * MAX_AXIS);
	int	mMinNegativeThreshold = -(int)(mMinAxisInputThreshold * MAX_AXIS);

};

class ENGINE_API ModuleInput : public Module
{
public:

	ModuleInput();
	virtual ~ModuleInput();

	bool Init() override;
	virtual update_status PreUpdate(float dt) override;
	bool CleanUp() override;

	KeyState GetKey(int id) const { return mKeyboard[id]; }
	bool GetKeyboardReciveInputs() const { return mKeyboardReceivedInput; }

	KeyState GetMouseKey(MouseKey id) const { return mMouse[id]; }
	void GetMouseMotion(int& x, int& y) const { x = mMouseMotionX; y = mMouseMotionY; }
	float2 GetLocalMousePosition() const { return float2(static_cast<float>(mMouseLocalPositionX), static_cast<float>(mMouseLocalPositionY)); }
	float2 GetGlobalMousePosition() const { return float2(static_cast<float>(mMouseGlobalPositionX), static_cast<float>(mMouseGlobalPositionY)); }
	int GetMouseWheelMotion() const { return mWheelY; }
	bool GetMouseRecieveInputs() const { return mMouseReceivedInput; }
	bool isGamepadAvailable() const { return mGameController.mId != nullptr; }

	ButtonState	GetGameControllerButton(int id) const { return mGameController.mButtons[id]; }
	ButtonState	GetGameControllerTrigger(int id) const { return mGameController.mTriggers[id]; }
	AxisState GetGameControllerAxis(int id) const { return mGameController.mAxis[id]; }
	float GetGameControllerAxisValue(int id) const;
	//int	GetGameControllerAxisRaw(int id) const;
	//int	GetGameControllerAxisInput(int id) const;
	void SetGameControllerRumble(unsigned int low_freq, unsigned int high_freq, unsigned int time);
	bool GetGameControllerReceivedInputs() const { return mGameControllerReceivedInput; }


	void HandleGameControllerInput();

protected:

	KeyState mMouse[MouseKey::NUM_MOUSE_BUTTONS] = {};
	KeyState* mKeyboard = 0;
	int mMouseMotionX = 0;
	int mMouseMotionY = 0;
	int mMouseLocalPositionX = 0; //Local is the position inside the window and global the position relative to the desktop
	int mMouseLocalPositionY = 0;
	int mMouseGlobalPositionX = 0;
	int mMouseGlobalPositionY = 0;

	int mWheelY = 0;

	bool mKeyboardReceivedInput = false;
	bool mMouseReceivedInput = false;
	bool mGameControllerReceivedInput = false;

	GameController  mGameController;
};

#endif /* _MODULE_INPUT_H_ */
