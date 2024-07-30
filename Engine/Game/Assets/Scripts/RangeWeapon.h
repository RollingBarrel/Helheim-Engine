#pragma once
#include "Weapon.h"
#include "Math/float3.h"

class GameObject;
class ColorGradient;


class RangeWeapon : public Weapon
{

public:
	RangeWeapon();
	~RangeWeapon();

	float GetAttackDuration() override { return mAttackDuration; }
	int GetCurrentAmmo() { return mCurrentAmmo; }
	int GetMaxAmmo() { return mMaxAmmo; }
	void Shoot(const float3& position, const float3& direction, const ColorGradient& trailGradient);
	virtual void Reload() = 0;

protected:
	virtual void PlayHitSound() = 0;
	
	float mBulletSpeed = 10.0f;
	float mBulletSize = 1.0f;

	GameObject* mFire = nullptr;

};

