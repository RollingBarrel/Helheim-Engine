#pragma once
#include "Weapon.h"

class GameObject;

class RangeWeapon : public Weapon
{

public:
	RangeWeapon();
	~RangeWeapon();

	float GetAttackDuration() override { return mAttackDuration; }
	int GetCurrentAmmo() { return mCurrentAmmo; }
	int GetMaxAmmo() { return mMaxAmmo; }

	virtual void Reload() = 0;

protected:
	GameObject* mFire = nullptr;
	virtual void PlayHitSound() = 0;
};

