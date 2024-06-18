#pragma once
#include "MeleeWeapon.h"

class TrailComponent;

class Hammer : public MeleeWeapon
{
public:
	Hammer();
	~Hammer();

	void Enter() override;
	void Attack() override;
	void Exit() override;

private:

	TrailComponent* mTrail = nullptr;
	const float mEnergyCost = 10.0f;
	const float mCooldownModifier = 1.5f;
};
