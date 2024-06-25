#pragma once
#include <Script.h>
#include "Macros.h"

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

	void Teleport();


	void OnCollisionEnter(CollisionData* collisionData);

private:
	
	GameObject* mDestination;
	BoxColliderComponent* mCollider;
};

