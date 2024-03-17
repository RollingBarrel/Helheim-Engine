#pragma once

enum class HelheimEventType : int
{
	UNKNOWN = 0,
	PRESSED_PLAY,
	PRESSED_PAUSE,
	PRESSED_RESUME,
	PRESSED_STEP,
	PRESSED_STOP,
	GAMEOBJECT_CREATED,
	GAMEOBJECT_DESTROYED,
	COUNT
};


struct HelheimEvent
{
public:
	HelheimEvent(HelheimEventType type);

	HelheimEventType mType = HelheimEventType::UNKNOWN;

};
