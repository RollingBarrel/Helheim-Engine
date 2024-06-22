#pragma once
#include "MeleeWeapon.h"

class TrailComponent;

class Hammer : public MeleeWeapon
{
public:
	Hammer();
	~Hammer();

	void Enter() override;
	void Attack(float time) override;
	void Exit() override;

private:
	void PlayHitSound();
	TrailComponent* mTrail = nullptr;
	const float mEnergyCost = 10.0f;
	const float mCooldownModifier = 1.5f;
};
