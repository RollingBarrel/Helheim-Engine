#pragma once
#include <Script.h>
#include "Macros.h"
#include "float3.h"


class BoxColliderComponent;
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
	
	float3 LerpPosition(float duration);
	GameObject* mDestination;
	BoxColliderComponent* mCollider;
	float mDuration = 4.0f; // Expose as script variable
	float mEnterDuration = 1.5f;
	float mCurrentTime = 0.0f;
	bool mIsTriggered = false;
	bool mIsEntering = false;
	bool mIsExiting = false;
	float mDistance = 0.0f;
	float3 mDirection;
	float3 mStartPos;
	GameObject* mPlayer = nullptr;
};

