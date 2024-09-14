#pragma once
#include <Script.h>
#include "Macros.h"

struct CollisionData;
class BoxColliderComponent;
class MeshRendererComponent;
GENERATE_BODY(AreaDoors);
class AreaDoors : public Script
{
	FRIEND(AreaDoors)
public:
	AreaDoors(GameObject* owner);	
	void Start() override;
	void Update() override;

	void CloseDoors(bool close);

private:
	void OnCollisionEnter(CollisionData* collisionData);

	GameObject* mDoorEnter = nullptr;
	GameObject* mDoorExit = nullptr;

	GameObject* mDoorEnterEmiBorder = nullptr;
	MeshRendererComponent* mBorderRenderEnter = nullptr;
	GameObject* mDoorExitEmiBorder = nullptr;
	MeshRendererComponent* mBorderRenderExit = nullptr;

	GameObject* mDoorEnterEmiTop = nullptr;
	MeshRendererComponent* mTopRenderEnter = nullptr;
	GameObject* mDoorExitEmiTop = nullptr;
	MeshRendererComponent* mTopRenderExit = nullptr;

	BoxColliderComponent* mCollider = nullptr;
	bool mHasEntered = false;

	float3 mClosedColor = float3(255.0f, 37.0f, 0.0f);
	float3 mOpenColor = float3(0.0f, 255.0f, 0.0f);
};

