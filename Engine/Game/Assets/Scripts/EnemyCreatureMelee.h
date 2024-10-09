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
	void Death() override;

	void OnCollisionEnter(CollisionData* collisionData);

private:
	BoxColliderComponent* mCollider = nullptr;
	GameObject* mDashAttackVFX = nullptr;
	GameObject* mDashRightVFX = nullptr;
	GameObject* mDashLeftVFX = nullptr;
	GameObject* mDashIndicator = nullptr;
	GameObject* mDashIndicatorFinal = nullptr;

	bool mHit = false;
	bool mAttack = false;
	bool mAttackAudioPlayed = false;
};

