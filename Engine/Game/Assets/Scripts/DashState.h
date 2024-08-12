#pragma once
#include "State.h"
#include "float3.h"

class TrailComponent;
class ColorGradient;
class GameObject;

class DashState : public State
{
public:
	explicit DashState(PlayerController* player, float cooldown);
	~DashState();

	StateType HandleInput() override;
	void Update() override;

	void Enter() override;
	void Exit() override;

	StateType GetType() override;

	void DashFX(const float3& startposition, const float3& endPosition, float size, const ColorGradient* gradient);

private:
	
	float mDashTimer = 0.0f;
	float mDashDuration = 0.5f;

	float3 mInitialPos;

	TrailComponent* mDashTrail = nullptr;
	GameObject* mDashVFX;
};