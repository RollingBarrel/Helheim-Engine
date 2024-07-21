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
	ULTIMATE,
	NONE,
};

class State
{
public:
	explicit State(PlayerController* player, float cooldown);
	~State();

	virtual StateType HandleInput();
	virtual void Update();

	virtual void Enter();
	virtual void Exit();

	virtual StateType GetType();

	virtual bool IsReady();
	void ResetCooldown() { mStateTimer = 0.0f; }
	void SetCooldown(float cooldown) { mStateCooldown = cooldown; }

protected:
	PlayerController* mPlayerController = nullptr;

	float mStateCooldown = 0.0f;
	float mStateTimer = 10.0f;

private:
};