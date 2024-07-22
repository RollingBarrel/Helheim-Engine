#pragma once
#include <Script.h>
#include "Macros.h"
#include "float3.h"


class BoxColliderComponent;
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
	
	float3 LerpPosition();
	GameObject* mDestination;
	BoxColliderComponent* mCollider;
	float mDuration = 4.0f; // Expose as script variable
	float mCurrentTime = 0.0f;
	bool mIsTriggered = false;
	float mDistance = 0.0f;
	float3 mDirection;
	float3 mStartPos;
	GameObject* mPlayer = nullptr;
	PlayerController* mPlayerController = nullptr;
};

