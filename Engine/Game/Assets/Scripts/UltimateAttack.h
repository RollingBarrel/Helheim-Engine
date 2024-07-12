#pragma once
#include "Script.h"
#include "Macros.h"
#include "TimerScript.h"

GENERATE_BODY(UltimateAttack);

class BoxColliderComponent;
class GameObject;
struct CollisionData;

class UltimateAttack : public Script
{
	FRIEND(UltimateAttack)
public:
	UltimateAttack(GameObject* owner);
	~UltimateAttack();

	void Start() override;
	void Update() override;

private:

	void OnCollisionEnter(CollisionData* collisionData);
	BoxColliderComponent* mCollider = nullptr;

	TimerScript mDamageTimer;

	float mInterval;
	float mDamageTick;
};

