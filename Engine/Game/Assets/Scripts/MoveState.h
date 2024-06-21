#pragma once
#include "State.h"

#include "float3.h"
#include "float2.h"

class MoveState : public State
{
public:
	explicit MoveState(PlayerController* player, float cooldown);
	~MoveState();

	StateType HandleInput() override;
	void Update() override;

	void Enter() override;
	void Exit() override;

	StateType GetType() override;

private:
	void DoAnimation();
	void PlayAudio();

	float2 SetMovingDirection();

	float3 mCameraFront;
	float3 mMoveDirection;

	float mStepTimer = 0.0f;
	float mStepCooldown = 0.5f;

	float mDashTimer = 0.0f;
};