#pragma once
#include "Script.h"
#include "Macros.h"
#include "Geometry/AABB.h"
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

	float mInterval = 0.0f;
	float mDamageTick = 0.0f;
	
	void ResetTimer() { mExpansionTimer.Reset(); }
private:

	void SetLength(float targetPercent, float speed);
	void SetFinalPoint();
	void OnCollisionEnter(CollisionData* collisionData);
	BoxColliderComponent* mCollider = nullptr;

	GameObject* mLaserGO = nullptr;
	GameObject* mLinesGO = nullptr;
	GameObject* mFinalPoint1 = nullptr;
	GameObject* mFinalPoint2 = nullptr;
	GameObject* mFinalPoint3 = nullptr;
	TimerScript mDamageTimer;
	TimerScript mExpansionTimer;
	float mLengthPercent = 10.0f;

	AABB mNoDamageArea;
};

