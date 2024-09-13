#pragma once
#include <Script.h>
#include "Macros.h"
#include "float3.h"


class BoxColliderComponent;
class AnimationComponent;
struct CollisionData;

GENERATE_BODY(Teleporter);
class Teleporter : public Script
{
	FRIEND(Teleporter)

public:

	Teleporter(GameObject* owner);
	~Teleporter();

	void Start() override;
	void Update() override;


	

	void OnCollisionEnter(CollisionData* collisionData);

private:
	
	float3 LerpPosition(float duration, float3 startPos);

	BoxColliderComponent* mCollider;
	float mDuration;
	float mEnterDuration = 1.5f;
	float mCurrentTime = 0.0f;
	bool mIsTriggered = false;
	bool mIsEntering = false;
	bool mIsExiting = false;

	bool mIsAtStart = true;

	float mDistance = 0.0f;
	float3 mCurrentDirection;

	float3 mStartPos;
	float3 mEndPos;
	float3 mCameraDif;
	float3 mDeltaCamera;
	float3 mFirstPlayerPos;
	GameObject* mBattleArea = nullptr;
	GameObject* mPlayer = nullptr;
	GameObject* mCamera = nullptr;
	AnimationComponent* mPlayerAnimation = nullptr;

	int mElevatorAudio = -1;
};

