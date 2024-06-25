#pragma once
#include <Script.h>

struct CollisionData;
class BoxColliderComponent;

class ElectricTrapController :
    public Script
{
public:
	ElectricTrapController(GameObject* owner);
	~ElectricTrapController() {}
	void Update() override;
	void Start() override;
private:
	void OnCollisionEnter(CollisionData* collisionData);

	BoxColliderComponent* mCollider = nullptr;

	float mArea = 1.0f;

};

