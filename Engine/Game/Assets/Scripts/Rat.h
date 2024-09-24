#pragma once
#include "Enemy.h"

GENERATE_BODY(Rat);
class Rat : public Enemy
{
	FRIEND(Rat)

public:
	Rat(GameObject* owner) : Enemy(owner) {}
	~Rat() {}

	void Update() override;
	void Idle() override;
	void Flee() override;
	float mFleeRadius = 2.0f;
};

