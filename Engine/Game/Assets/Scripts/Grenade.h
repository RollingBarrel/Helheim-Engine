#pragma once
#include "Script.h"
#include "Macros.h"
#include "float3.h"
#include <vector>

GENERATE_BODY(Grenade);

class GameObject;

enum class GRENADE_STATE {
	INACTIVE,
	AIM,
	MOVEMENT,
	EXPLOTION_START,
	EXPLOTION_END
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
	void Explotion();
	void BlackHole();
	void PullCloser(std::vector<GameObject*> enemies);
	void EndExplotion();

	std::vector<GameObject*> GetAffectedEnemies();

	GRENADE_STATE mState = GRENADE_STATE::INACTIVE;
	// Player-depends status
	float mGrenadeDPS = 1.0f;
	float mGrenadeDuration = 4.0f;
	float mGrenadeCurrentTime = mGrenadeDuration;
	float mGrenadeRadius = 3.0f; // Explotion area

	float3 mInitialPosition = float3(0, 0, 0); // Init destination to 0,0,0
	float3 mDestination = float3(0, 0, 0); // Init destination to 0,0,0
	float3 mCurrentPosition;
	float mTotalDistance;
	float mCurrentDistance;

	float mSpeed = 10.0f; // Speed of movement
	float mArcHeight = 2.0f; // Maximum height of the arc
	float mThreshold = 0.3f; // Distance threshold

	float mTimeAccumulator = 0.0f;
	const float mPullInterval = 0.3f;

	GameObject* mFire = nullptr;
};