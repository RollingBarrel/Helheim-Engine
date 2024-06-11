#pragma once
#include "State.h"

class AttackState : public State
{
public:
	explicit AttackState(PlayerController* player);
	~AttackState();

	StateType HandleInput() override;
	void Update() override;

	void Enter() override;
	void Exit() override;

	StateType GetType() override;

	void PlayAudio() override;
};