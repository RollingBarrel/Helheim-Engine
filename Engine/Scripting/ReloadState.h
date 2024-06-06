#pragma once
#include "State.h"

class ReloadState : public State
{
public:
	ReloadState(PlayerController* player);
	~ReloadState();

	StateType HandleInput() override;
	void Update() override;

	void Enter() override;
	void Exit() override;

private:
	float mReloadTimer = 0;
	bool mReloaded = false;
};