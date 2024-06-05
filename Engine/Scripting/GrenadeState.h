#pragma once
#include "State.h"

class GrenadeState : public State
{
public:
	GrenadeState(PlayerController* player);
	~GrenadeState();

	StateType HandleInput() override;
	void Update() override;

	void Enter() override;
	void Exit() override;

private:

};