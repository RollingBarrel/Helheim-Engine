#pragma once
#include <Script.h>
#include "Macros.h"

struct CollisionData;
class BoxColliderComponent;
class GameObject;

GENERATE_BODY(ElectricTrapGroupController);
class ElectricTrapGroupController :
	public Script
{
	FRIEND(ElectricTrapGroupController)
public:
	ElectricTrapGroupController(GameObject* owner);
	~ElectricTrapGroupController();

	void Start() override;
	void Update() override;
	void OnCollisionEnter(CollisionData* collisionData);

private:
	void ActiveTrap(bool active);

	BoxColliderComponent* mCollider = nullptr;
	GameObject* mTraps = nullptr;
};

