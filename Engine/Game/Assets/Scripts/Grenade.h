#pragma once
#include "Script.h"
#include "Macros.h"
#include "float3.h"
#include "TimerScript.h"
#include <vector>

GENERATE_BODY(Grenade);

class GameObject;

enum class GRENADE_STATE
{
	MOVE,
	BLACK_HOLE,
	EXPLOSION,
	NONE
};

class Grenade : public Script
{
	FRIEND(Grenade)
public:
	Grenade(GameObject* owner) : Script(owner) {}
	~Grenade() {}

	void Start() override;
	void Update() override;

	void ThrowGrenade(float3 initialPosition, float3 destination);

	float GetGrenadeRadius();
private:
	void MoveToTarget();
	void CalculateTrajectory();
	float3  CalculatePositionAtTime(float t);
	void Explosion();
	void BlackHole();
	void MakeDamage(std::vector<GameObject*> enemies);
	void PullCloser(std::vector<GameObject*> enemies);
	void EndExplosion();
	void GetAffectedEnemies(std::vector<GameObject*>& affectedEnemies) const;

	GRENADE_STATE mState = GRENADE_STATE::NONE;
	// Player-depends status
	float mGrenadeDamage = 50.0f;
	TimerScript mBlackHoleTimer; 
	TimerScript mExplosionTimer;
	float mExplosionDuration = 2.0f;
	float mBlackHoleDuration = 2.0f;
	float mGrenadeRadius = 5.0f; // Explosion area

	// Before explotion
	float3 mInitialPosition = float3(0, 0, 0); 
	float3 mDestination = float3(0, 0, 0);
	float3 mCurrentPosition;

	float mTotalDistance = 0.0f;
	float mCurrentDistance = 0.0f;
	float3 mVelocity;

	// Very slow <0.5 -- 1 -- 2.5> Very fast
	float mTrajectorySpeedFactor = 1.5f;

	float mFlightTime = 0.0f;
	float mElapsedTime = 0.0f;


	GameObject* mTrail = nullptr;
	GameObject* mBlackHoleSFX = nullptr;
	GameObject* mExplosionSFX = nullptr;
	GameObject* mSphere = nullptr;

	int mExplosionAudio = -1;
};