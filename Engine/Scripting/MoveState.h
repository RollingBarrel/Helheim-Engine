#pragma once
#include "State.h"

#include "float3.h"

class MoveState : public State
{
public:
	MoveState(PlayerController* player);
	~MoveState();

	StateType HandleInput() override;
	void Update() override;

	void Enter() override;
	void Exit() override;

private:
	float3 mCameraFront;
};