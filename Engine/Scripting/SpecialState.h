#pragma once
#include "State.h"

class SpecialState : public State
{
public:
	SpecialState(PlayerController* player);
	~SpecialState();

	StateType HandleInput() override;
	void Update() override;

	void Enter() override;
	void Exit() override;

private:

};