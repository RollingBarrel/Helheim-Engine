#pragma once
#include "Enemy.h"
#include "Geometry/Ray.h"

struct CollisionData;
class BoxColliderComponent;

GENERATE_BODY(EnemyRobotMelee);
class EnemyRobotMelee : public Enemy
{
	FRIEND(EnemyRobotMelee)

public:
	EnemyRobotMelee(GameObject* owner) : Enemy(owner) {}
	~EnemyRobotMelee() {}

private:
	void Start() override;
	void Charge() override;
	void Attack() override;

	void PlayStepAudio();
	void PlayMeleeAudio();

	float mMeeleDamage = 10.0f;
	GameObject* mSwordTrail = nullptr;
	// Step Sound
	float mStepTimer = 0.0f;
	float mStepCooldown = 0.5f;
};

