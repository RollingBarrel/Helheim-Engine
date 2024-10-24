#pragma once
#include "State.h"

#include "float3.h"
#include "float2.h"
#include "TimerScript.h"

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

	float ComputeMoveAnge(float3 mouseDir);
	std::string GetTriggerFromAngle(float angle);

	float3 mCameraFront;
	float3 mMoveDirection;

	TimerScript mStepTimer;
	float mStepCooldown = 0.3f;

	float mDashTimer = 0.0f;
};