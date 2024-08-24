#pragma once
#include "Script.h"
#include "Macros.h"
#include "float3.h"
#include <vector>
#include "TimerScript.h"

GENERATE_BODY(Grenade);

class GameObject;

enum class GRENADE_STATE {
	INACTIVE,
	MOVEMENT,
	EXPLOSION_START,
};

class Grenade : public Script
{
	FRIEND(Grenade)
public:
	Grenade(GameObject* owner);
	~Grenade();

	void Start() override;
	void Update() override;

	void SetPositionDestination(float3 initialPosition, float3 destination);

	float GetGrenadeRadius();
private:
	void MoveToTarget();
	void CalculateTrajectory();
	float3  CalculatePositionAtTime(float t);
	void Explosion();
	void BlackHole();
	void PullCloser(std::vector<GameObject*> enemies);
	void EndExplosion();

	std::vector<GameObject*> GetAffectedEnemies();

	GRENADE_STATE mState = GRENADE_STATE::INACTIVE;
	// Player-depends status
	float mGrenadeDPS = 1.0f;
	float mGrenadeDuration = 4.0f;
	TimerScript mExplosionTimer;
	float mGrenadeRadius = 5.0f; // Explosion area

	// Before explotion
	float3 mInitialPosition = float3(0, 0, 0); 
	float3 mDestination = float3(0, 0, 0);
	float3 mCurrentPosition;

	float mTotalDistance;
	float mCurrentDistance = 0.0f;
	float3 mVelocity;

	// Very slow <0.5 -- 1 -- 2.5> Very fast
	float mTrajectorySpeedFactor = 1.5f;

	float mFlightTime = 0.0f;
	float mElapsedTime = 0.0f;


	GameObject* mGrenade = nullptr;
	GameObject* mExplosionSFX = nullptr;
};