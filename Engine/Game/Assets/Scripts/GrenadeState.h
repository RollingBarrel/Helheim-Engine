#pragma once
#include "State.h"

class GrenadeState : public State
{
public:
	explicit GrenadeState(PlayerController* player, float cooldown);
	~GrenadeState();

	StateType HandleInput() override;
	void Update() override;

	void Enter() override;
	void Exit() override;

	StateType GetType() override;

private:
	bool mThrowGrenade = false;
};