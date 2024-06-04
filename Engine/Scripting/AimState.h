#pragma once
#include "State.h"

class AimState : public State
{
public:
	AimState(PlayerController* player);
	~AimState();

	StateType HandleInput() override;
	void Update() override;

	void Enter() override;
	void Exit() override;

private:

};