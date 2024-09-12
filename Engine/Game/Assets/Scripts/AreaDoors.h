#pragma once
#include <Script.h>
#include "Macros.h"

struct CollisionData;
class BoxColliderComponent;
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

	GameObject* mDoor1 = nullptr;
	GameObject* mDoor2 = nullptr;
	BoxColliderComponent* mCollider = nullptr;
	bool mHasEntered = false;
};

