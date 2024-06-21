#pragma once
#include "MeleeWeapon.h"

class TrailComponent;

class Hammer : public MeleeWeapon
{
public:
	Hammer(BoxColliderComponent* collider);
	~Hammer();

	void Enter() override;
	void Attack(float time) override;
	void Exit() override;

private:

	TrailComponent* mTrail = nullptr;
	const float mEnergyCost = 10.0f;
	const float mCooldownModifier = 1.5f;
};
