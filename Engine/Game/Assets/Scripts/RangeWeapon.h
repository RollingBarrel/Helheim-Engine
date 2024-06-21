#pragma once
#include "Weapon.h"

class RangeWeapon : public Weapon
{

public:
	RangeWeapon();
	~RangeWeapon();

	float GetAttackTime() override { return mAttackTime; }
	int GetCurrentAmmo() { return mCurrentAmmo; }
	int GetMaxAmmo() { return mMaxAmmo; }

	virtual void Reload() = 0;

protected:


};

