#pragma once

class GameObject;

enum class HelheimEventType : int
{
	UNKNOWN = 0,
	PRESSED_PLAY,
	PRESSED_PAUSE,
	PRESSED_RESUME,
	PRESSED_STEP,
	PRESSED_STOP,
	PRESSED_UI_BUTTON,
	GAMEOBJECT_CREATED,
	GAMEOBJECT_DESTROYED,
	COUNT
};

struct EventData
{
	GameObject* mGameObject = nullptr;
};

struct HelheimEvent
{
	HelheimEvent(HelheimEventType type);
	HelheimEvent(HelheimEventType type, EventData* data);

	HelheimEventType mType = HelheimEventType::UNKNOWN;
	EventData* mData = nullptr;
};
