#pragma once
#include "Script.h"
#include "Macros.h"
#include "float3.h"

GENERATE_BODY(Grenade);

class Grenade : public Script
{
	FRIEND(Grenade)
public:
	Grenade(GameObject* owner);
	Grenade(GameObject* owner, float dps, float duration, float area);
	~Grenade();

	void Start() override;
	void Update() override;

	void Explotion();
	void EndExplotion();

	void SetDestionation(float3 destination);

private:
	// Basic status
	float mGrenadeDPS = 1.0f;
	float mGrenadeDuration = 4.0f;
	float mGrenadeCurrentTime = mGrenadeDuration;
	float mGrenadeArea = 2.0f;

	float mGrenadeCoolDown = 15.0f;
	float mGrenadeThrowSpeed = 1.0f;

	float3 mDestination = float3(0, 0, 0); // Init destination to 0,0,0

	bool mExplotionStart = false;
};