#pragma once
#include "State.h"

class AttackState : public State
{
public:
	AttackState(PlayerController* player);
	~AttackState();

	StateType HandleInput() override;
	void Update() override;

	void Enter() override;
	void Exit() override;

private:

};