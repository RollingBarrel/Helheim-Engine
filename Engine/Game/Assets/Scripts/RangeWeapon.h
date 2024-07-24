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
	virtual void PlayHitSound() = 0;
	
	float mBulletSpeed = 10.0f;
	float mBulletSize = 1.0f;

	GameObject* mFire = nullptr;

};

