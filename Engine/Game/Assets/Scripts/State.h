#pragma once

class PlayerController;

enum StateType
{
	IDLE,
	DASH,
	MOVE,
	AIM,
	ATTACK,
	SWITCH,
	GRENADE,
	SPECIAL,
	RELOAD,
	NONE,
};

class State
{
public:
	explicit State(PlayerController* player);
	~State();

	virtual StateType HandleInput();
	virtual void Update();

	virtual void Enter();
	virtual void Exit();

	virtual StateType GetType();

	virtual void PlayAudio();

protected:
	PlayerController* mPlayerController = nullptr;

private:
};