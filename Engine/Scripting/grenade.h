#pragma once
#include "Script.h"
#include "Macros.h"
#include "float3.h"
#include <vector>

GENERATE_BODY(Grenade);

class GameObject;

class Grenade : public Script
{
	FRIEND(Grenade)
public:
	Grenade(GameObject* owner);
	~Grenade();

	void Start() override;
	void Update() override;

	void SetDestionation(float3 destination);

	void SetGrenadeParameters(float dps, float duration, float area);
private:
	void Explotion();
	void BlackHole();
	void PullCloser(std::vector<GameObject*> enemies);
	void EndExplotion();

	std::vector<GameObject*> GetAffectedEnemies();

	// Player-depends status
	float mGrenadeDPS = 1.0f;
	float mGrenadeDuration = 4.0f;
	float mGrenadeCurrentTime = mGrenadeDuration;
	float mGrenadeRadius = 2.0f;

	float3 mDestination = float3(0, 0, 0); // Init destination to 0,0,0

	bool mExplotionStart = false;

	float mTimeAccumulator = 0.0f; 
	const float mPullInterval = 0.3f; 
};