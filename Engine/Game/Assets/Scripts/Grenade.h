#pragma once
#include "Script.h"
#include "Macros.h"
#include "Math/float3.h"

GENERATE_BODY(Grenade);

class Grenade : public Script
{
public:
	FRIEND(Grenade)
	Grenade(GameObject* owner, float3 destination);
	Grenade(GameObject* owner, float dps, float duration, float area, float3 destination);
	~Grenade();

	void Start() override;
	void Update() override;

private:
	// Basic status
	float mGrenadeDPS = 1.0f;
	float mGrenadeDuration = 4.0f;
	float mGrenadeArea = 2.0f;

	float mGrenadeCoolDown = 15.0f;
	float mGrenadeThrowSpeed = 1.0f;

	float3 mDestination = float3(0, 0, 0); // Init destination to 0,0,0

};