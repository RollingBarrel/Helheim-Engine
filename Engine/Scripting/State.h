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
	State(PlayerController* player);
	~State();

	virtual StateType HandleInput();
	virtual void Update();

	virtual void Enter();
	virtual void Exit();

	virtual StateType GetType();

protected:
	PlayerController* mPlayerController = nullptr;

private:
};