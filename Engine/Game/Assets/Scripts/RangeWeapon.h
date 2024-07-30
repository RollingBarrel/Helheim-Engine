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
	
	virtual void Reload() = 0;

protected:
	void Shoot(const float3& position, float maxSpread, const ColorGradient& trailGradient);
	float3 Spread(const float3& front, const float3& up, const float3& right, float maxSpread);
	virtual void PlayHitSound() = 0;
	
	float mBulletSpeed = 10.0f;
	float mBulletSize = 1.0f;
	float mBulletMaxSpread = 0.0f;

	GameObject* mFire = nullptr;

};

