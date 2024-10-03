#pragma once
#include "Enemy.h"

class BoxColliderComponent;
class DecalComponent;
class ParticleSystemComponent;
struct CollisionData;

GENERATE_BODY(Rat);
class Rat : public Enemy
{
	FRIEND(Rat)

public:
	Rat(GameObject* owner) : Enemy(owner) {}
	~Rat() {}

	void Start() override;
	void Update() override;

private:
	void Idle() override;
	void Flee() override;
	void Death() override;
	void OnCollisionEnter(CollisionData* collisionData);
	
	BoxColliderComponent* mCollider = nullptr;
	DecalComponent* mDecalComponent = nullptr;
	ParticleSystemComponent* mDeathParticles = nullptr;
	float mFleeRadius = 5.0f;
};

