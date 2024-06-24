#pragma once
#include "MeleeWeapon.h"

class TrailComponent;

class Hammer : public MeleeWeapon
{
public:
	Hammer(BoxColliderComponent* collider, TrailComponent* trail);
	~Hammer();

protected:
	void PlayHitSound();
	void ApplySpecialEffects(GameObject* enemy) override;

	float mHeavyDamage = 7;
	int mHeavyEnergyCost = 10;
	float mHeavyCooldownMultiplier = 1.5f;

};
