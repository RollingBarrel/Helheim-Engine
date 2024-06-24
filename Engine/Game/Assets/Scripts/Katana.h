#pragma once
#include "MeleeWeapon.h"

class TrailComponent;

class Katana : public MeleeWeapon
{
public:
	Katana(BoxColliderComponent* collider, TrailComponent* trail);
	~Katana();

protected:
	void PlayHitSound();
	void ApplySpecialEffects(GameObject* enemy) override;

	float mFastDamage = 4;
	int mFastEnergyCost = 10;
	float mFastCooldownMultiplier = 0.5f;
};
