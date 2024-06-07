#pragma once
#include "State.h"

class AimState : public State
{
public:
	explicit AimState(PlayerController* player);
	~AimState();

	StateType HandleInput() override;
	void Update() override;

	void Enter() override;
	void Exit() override;

	StateType GetType() override;


private:
	float mAttackTimer = 0.0f;
	float mSpecialAttackTimer = 0.0f;
	float mGrenadeTimer = 0.0f;
	float mSwitchTimer = 0.0f;
};