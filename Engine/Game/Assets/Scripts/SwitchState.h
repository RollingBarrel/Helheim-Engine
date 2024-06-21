#pragma once
#include "State.h"

class PlayerController;

class SwitchState : public State
{
public:
	explicit SwitchState(PlayerController* player);
	~SwitchState();

	StateType HandleInput() override;
	void Update() override;

	void Enter() override;
	void Exit() override;

	StateType GetType() override;

private:
	float mSwitchTimer = 0.0f;
};