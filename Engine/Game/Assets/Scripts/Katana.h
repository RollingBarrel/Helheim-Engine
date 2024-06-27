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
	void HitEffect(GameObject* enemy) override;
};
