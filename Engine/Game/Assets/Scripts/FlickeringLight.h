#pragma once
#include <vector>
#include <Script.h>
#include "Macros.h"

GENERATE_BODY(FlickeringLight);

class Component;
class MeshRendererComponent;
class ParticleSystemComponent;

struct Blackout 
{
	float mTime;
	float mDuration;

	Blackout(float time, float duration) : mTime(time), mDuration(duration) {}
	Blackout() 
	{
		mTime = 0.0f;
		mDuration = 0.0f;
	}
};

class FlickeringLight : public Script
{
	FRIEND(FlickeringLight)

public:
	FlickeringLight(GameObject* owner);

	void Update() override;
	void Start() override;

private:

	bool UpdateLightState();

	float mLoopDuration = 10.0f;
	float mStartingTime = 0.0f;
	float mTimeOffset = 0.0f;
	float mSpeed = 1.0f;

	float mTimer = 0.0f;
	bool mLightOn = true;

	//Blackouts
	Blackout mBlackout1;
	Blackout mBlackout2;
	Blackout mBlackout3;
	Blackout mBlackout4;
	Blackout mBlackout5;
	Blackout mBlackout6;
	Blackout mBlackout7;
	Blackout mBlackout8;
	Blackout mBlackout9;
	Blackout mBlackout10;

	std::vector<Blackout> flickering;

	Component* mLightComp = nullptr;
	MeshRendererComponent* mMeshRenderComp = nullptr;

	//Particle functionality
	ParticleSystemComponent* mParticles = nullptr;
	bool mRestartParticles = false;

};
