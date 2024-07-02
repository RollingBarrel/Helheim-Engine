#pragma once
#include <Script.h>
#include <vector>
#include "Macros.h"

struct CollisionData;
class BoxColliderComponent;
class GameObject;

GENERATE_BODY(ElectricTrapController);
class ElectricTrapController :
    public Script
{
	FRIEND(ElectricTrapController)
public:
	ElectricTrapController(GameObject* owner);
	~ElectricTrapController();
	void Update() override;
	void Start () override;
	void OnCollisionEnter(CollisionData* collisionData);

private:
	bool CheckIfCaptured(const GameObject* target);
	void DeactiveTrap();
	void ActiveTrap();

	BoxColliderComponent* mCollider = nullptr;
	std::vector<GameObject*> mInTrap;

	float mArea = 1.0f; 

	// Activation
	float mTimer = 0.0f;
	bool mIsActive = false;
	float mActivationInterval = 8.0f;
	float mActivationDuration = 4.0f;
	
	// Damage
	float mDamageAmount = 5.0f;
	float mSpeedReduction = 0.3f; // Reduce 70%
};

