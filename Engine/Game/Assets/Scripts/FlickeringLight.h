#pragma once
#include <vector>
#include <Script.h>
#include "Macros.h"

GENERATE_BODY(FlickeringLight);

class SpotLightComponent;
class PointLightComponent;

struct Blackout {
	float mTime;
	float mDuration;

	Blackout(float time, float duration) : mTime(time), mDuration(duration) {}
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
	float mSpeed = 1.0f;

	float mTimer = 0.0f;
	bool mLightOn = true;
	float mLightIntensity = 0;

	std::vector<Blackout> flickering;

	SpotLightComponent* mSpotLight = nullptr;
	PointLightComponent* mPointLight = nullptr;

};
