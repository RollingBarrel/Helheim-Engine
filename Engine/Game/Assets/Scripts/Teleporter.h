#pragma once
#include <Script.h>
#include "Macros.h"
#include "float3.h"


class BoxColliderComponent;
class AnimationComponent;
class PlayerCamera;
class PlayerController;
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

	BoxColliderComponent* mCollider = nullptr;
	float mDuration = 0.0f;
	float mEnterDuration = 1.5f;
	float mCurrentTime = 0.0f;
	bool mIsTriggered = false;
	bool mIsEntering = false;
	bool mIsExiting = false;

	bool mIsAtStart = true;

	float mDistance = 0.0f;
	float3 mCurrentDirection = float3::zero;

	float3 mStartPos = float3::zero;
	float3 mEndPos = float3::zero;;
	float3 mFirstPlayerPos = float3::zero;;
	GameObject* mBattleArea = nullptr;
	GameObject* mPlayer = nullptr;
	AnimationComponent* mPlayerAnimation = nullptr;
	PlayerCamera* mPlayerCamera = nullptr;
	float mOriginalCameraDist = 0.0f;
	float mCameraDif = 0.0f;

	PlayerController* mPlayerController = nullptr;

	int mElevatorAudio = -1;
	bool mCloseAudioPlayed = false;
};

