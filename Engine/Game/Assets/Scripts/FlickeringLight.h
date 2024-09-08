#pragma once
#include <vector>
#include <Script.h>
#include "Macros.h"

GENERATE_BODY(FlickeringLight);

class SpotLightComponent;
class PointLightComponent;

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
	Blackout b1;
	Blackout b2;
	Blackout b3;
	Blackout b4;
	Blackout b5;
	Blackout b6;
	Blackout b7;
	Blackout b8;
	Blackout b9;
	Blackout b10;
	Blackout b11;
	Blackout b12;
	Blackout b13;
	Blackout b14;
	Blackout b15;
	Blackout b16;
	Blackout b17;
	Blackout b18;
	Blackout b19;
	Blackout b20;

	std::vector<Blackout> flickering;

	SpotLightComponent* mSpotLight = nullptr;
	PointLightComponent* mPointLight = nullptr;

};
