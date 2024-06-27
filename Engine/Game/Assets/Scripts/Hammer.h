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
	void HitEffect(GameObject* enemy) override;
};
