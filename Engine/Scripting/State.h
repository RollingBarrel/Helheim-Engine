#pragma once

class PlayerController;

enum StateType
{
	IDLE,
	DASH,
	MOVE,
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

protected:
	PlayerController* mPlayerController = nullptr;

private:
};