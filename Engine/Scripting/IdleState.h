#pragma once
#include "State.h"

class IdleState : public State
{
public:
	IdleState(PlayerController* player);
	~IdleState();

	StateType HandleInput() override;
	void Update() override;

	void Enter() override;
	void Exit() override;

	StateType GetType() override;

private:

};