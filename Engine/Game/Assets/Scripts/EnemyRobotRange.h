#pragma once
#include "Enemy.h"
#include "Geometry/Ray.h"

class AnimationStateMachine;
class GameObject;
class BoxColliderComponent;
struct CollisionData;

GENERATE_BODY(EnemyRobotRange);
class EnemyRobotRange : public Enemy
{
	FRIEND(EnemyRobotRange)

public:
	EnemyRobotRange(GameObject* owner) : Enemy(owner) {}
	~EnemyRobotRange() {}

private:
	void Start() override;
	void Attack() override;
	void RangeAttack();
	void TakeDamage(float damage) override;
	void Death() override;

	void OnCollisionEnter(CollisionData* collisionData);

	BoxColliderComponent* mCollider = nullptr;
	float mRangeDamage = 15.0f;
	float mBulletSpeed = 15.0f;
	float mTimerDisengage = 0.0f;


	GameObject* mBulletOrigin = nullptr;

};

