#pragma once
#include "Weapon.h"

class RangeWeapon : public Weapon
{

public:
	RangeWeapon();
	~RangeWeapon();

	int GetCurrentAmmo() { return mCurrentAmmo; }
	int GetMaxAmmo() { return mMaxAmmo; }

	virtual void Reload() = 0;

protected:


};

