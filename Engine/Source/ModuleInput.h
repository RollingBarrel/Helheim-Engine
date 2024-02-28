#ifndef _MODULE_INPUT_H_
#define _MODULE_INPUT_H_

#include "Module.h"
#include "Globals.h"
#include "SDL_scancode.h"

enum class KeyState : unsigned char
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

enum MouseKey {
	BUTTON_LEFT,
	BUTTON_MIDDLE,
	BUTTON_RIGHT,
	BUTTON_X1,
	BUTTON_X2,

	NUM_MOUSE_BUTTONS
};

class ModuleInput : public Module
{
public:

	ModuleInput();
	~ModuleInput();

	bool Init() override;
	update_status PreUpdate(float dt) override;
	bool CleanUp() override;

	KeyState GetKey(int id) const { return keyboard[id]; }
	KeyState GetMouseKey(MouseKey id) const { return mouse[id]; }
	void GetMouseMotion(int& x, int& y) const { x = mMouseMotionX; y = mMouseMotionY; }
	void GetMousePosition(int& x, int& y) const { x = mMousePositionX; y = mMousePositionY; }
	int GetMouseWheelMotion() const { return wheelY; }
private:
	KeyState mouse[MouseKey::NUM_MOUSE_BUTTONS] = {};
	KeyState* keyboard = NULL;
	int mMouseMotionX = 0;
	int mMouseMotionY = 0;
	int mMousePositionX = 0;
	int mMousePositionY = 0;

	int wheelY = 0;
};

#endif /* _MODULE_INPUT_H_ */
