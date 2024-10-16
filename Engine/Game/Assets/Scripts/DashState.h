#pragma once
#include "State.h"
#include "float3.h"

class TrailComponent;
class ColorGradient;
class GameObject;
class ParticleSystemComponent;
class DashVFX;

class DashState : public State
{
public:
	explicit DashState(PlayerController* player, float cooldown);
	~DashState();

	StateType HandleInput() override;
	void Update() override;

	void Enter() override;
	void Exit() override;

	bool IsReady() override;

	StateType GetType() override;

private:
	
	float mDashTimer = 0.0f;
	float mDashDuration = 0.5f;

	GameObject* mDashVFX;
	GameObject* mCharacterMesh;
	GameObject* mPistolMesh;
};