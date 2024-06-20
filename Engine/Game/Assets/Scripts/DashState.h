#pragma once
#include "State.h"

class DashState : public State
{
public:
	explicit DashState(PlayerController* player);
	~DashState();

	StateType HandleInput() override;
	void Update() override;

	void Enter() override;
	void Exit() override;

	StateType GetType() override;

private:

	void PlayAudio() override;

	float mDashTimer = 0.0f;
	float mDashDuration = 0.5f;
};