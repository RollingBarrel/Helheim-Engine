#pragma once
#include "State.h"

class ReloadState : public State
{
public:
	explicit ReloadState(PlayerController* player);
	~ReloadState();

	StateType HandleInput() override;
	void Update() override;

	void Enter() override;
	void Exit() override;

	StateType GetType() override;

private:
	float mReloadTimer = 0.0f;
};