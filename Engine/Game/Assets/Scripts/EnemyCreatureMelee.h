#pragma once
#include "Enemy.h"

class BoxColliderComponent;
struct CollisionData;

GENERATE_BODY(EnemyCreatureMelee);
class EnemyCreatureMelee : public Enemy
{
	FRIEND(EnemyCreatureMelee)

public:
	EnemyCreatureMelee(GameObject* owner) : Enemy(owner) {}
	~EnemyCreatureMelee() {}

	void Start() override;
	void Update() override;
	void Chase() override;
	void Charge() override;
	void Attack() override;
	void TakeDamage(float damage) override;

	void OnCollisionEnter(CollisionData* collisionData);

private:

	BoxColliderComponent* mCollider = nullptr;
	bool mHit = false;
	bool mAudioPlayed = false;
};

