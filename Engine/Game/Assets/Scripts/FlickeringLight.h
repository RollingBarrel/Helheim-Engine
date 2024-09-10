#pragma once
#include <vector>
#include <Script.h>
#include "Macros.h"

GENERATE_BODY(FlickeringLight);
class Component;
class MeshRendererComponent;

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

	void UpdateLightState();

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
	Blackout mBlackout11;
	Blackout mBlackout12;
	Blackout mBlackout13;
	Blackout mBlackout14;
	Blackout mBlackout15;
	//Blackout mBlackout16;
	//Blackout mBlackout17;
	//Blackout mBlackout18;
	//Blackout mBlackout19;
	//Blackout mBlackout20;

	std::vector<Blackout> flickering;

	Component* mLightComp = nullptr;
	MeshRendererComponent* mMeshRenderComp = nullptr;

};
