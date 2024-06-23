#pragma once
#include "MeleeWeapon.h"

class TrailComponent;

class Hammer : public MeleeWeapon
{
public:
	Hammer(BoxColliderComponent* collider, TrailComponent* trail);
	~Hammer();
private:
	void PlayHitSound();
};
