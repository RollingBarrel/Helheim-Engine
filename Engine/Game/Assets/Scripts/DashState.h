#pragma once
#include "State.h"
#include "float3.h"

class TrailComponent;
class ColorGradient;
class GameObject;
class ParticleSystemComponent;

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

private:
	
	float mDashTimer = 0.0f;
	float mDashDuration = 0.5f;

	TrailComponent* mDashTrail = nullptr;

	ParticleSystemComponent* mDashParticles = nullptr;		//Active during the whole dash
	ParticleSystemComponent* mBegginingParticles = nullptr;	//Only active at the beggining of the dash (for responsiveness)
	ParticleSystemComponent* mEndingParticles = nullptr;	//Only active at the end of the dash

	GameObject* mDashVFX;
};