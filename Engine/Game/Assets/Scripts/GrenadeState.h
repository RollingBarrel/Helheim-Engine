#pragma once
#include "State.h"

class GrenadeState : public State
{
public:
	explicit GrenadeState(PlayerController* player, float cooldown) : State(player, cooldown) {}
	~GrenadeState() {}

	StateType HandleInput() override;
	void Update() override;

	void Enter() override;
	void Exit() override;

	bool IsReady() override;

	StateType GetType() override;

private:
	bool mThrowGrenade = false;
};