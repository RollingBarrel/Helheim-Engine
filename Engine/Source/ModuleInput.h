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
	update_status PreUpdate() override;
	bool CleanUp() override;

	KeyState GetKey(int id) const { return keyboard[id]; }
	KeyState GetMouseKey(MouseKey id) const { return mouse[id]; }
	void GetMouseMotion(int& x, int& y) const { x = mX; y = mY; }
	int GetMouseWheelMotion() const { return wheelY; }
private:
	KeyState mouse[MouseKey::NUM_MOUSE_BUTTONS] = {};
	KeyState* keyboard = NULL;
	int mX;
	int mY;
	int wheelY = 0;
};

#endif /* _MODULE_INPUT_H_ */