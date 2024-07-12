#pragma once
#include "State.h"
#include "TimerScript.h"

class UltimateState : public State
{
public:
	explicit UltimateState(PlayerController* player, float cooldown, float duration);
	~UltimateState();

	StateType HandleInput() override;
	void Update() override;

	void Enter() override;
	void Exit() override;

	StateType GetType() override;

	bool IsReady() override;

	TimerScript mTimer;

	float mUltimateDuration;
};

