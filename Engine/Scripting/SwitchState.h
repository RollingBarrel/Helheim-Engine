#pragma once
#include "State.h"

class SwitchState : public State
{
public:
	SwitchState(PlayerController* player);
	~SwitchState();

	StateType HandleInput() override;
	void Update() override;

	void Enter() override;
	void Exit() override;

private:

};