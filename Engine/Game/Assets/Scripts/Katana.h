#pragma once
#include "MeleeWeapon.h"

class TrailComponent;

class Katana : public MeleeWeapon
{
public:
	Katana(BoxColliderComponent* collider, TrailComponent* trail);
	~Katana();

private:
	void PlayHitSound();
};
