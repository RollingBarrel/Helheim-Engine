#pragma once
#include "Weapon.h"
class RangeWeapon : public Weapon
{

public:
	RangeWeapon(GameObject* owner) : Weapon(owner) {}

protected:

	GameObject* mShootPoint;

	float mCurrentAmmo;
	float mMaxAmmo;

};

