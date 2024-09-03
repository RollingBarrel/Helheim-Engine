#pragma once
#include "State.h"
#include "TimerScript.h"

class UltimateChargeState : public State
{
public:
	explicit UltimateChargeState(PlayerController* player, float cooldown, float duration);
	~UltimateChargeState();

	StateType HandleInput() override;
	void Update() override;

	void Enter() override;
	void Exit() override;

	StateType GetType() override;

	bool IsReady() override;

private:
	TimerScript mTimer;
	float mUltimateChargeDuration;
};

