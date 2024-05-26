#pragma once
#include "Weapon.h"
class RangeWeapon : public Weapon
{

private:

	GameObject* mProjectile;
	GameObject* mShootPoint;

	float mCurrentAmmo;
	float mMaxAmmo;

};

