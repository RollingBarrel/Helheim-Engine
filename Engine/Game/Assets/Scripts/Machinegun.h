#pragma once
#include "RangeWeapon.h"

class Machinegun : public RangeWeapon
{
public:
	Machinegun();
	~Machinegun();

	void Enter() override;
	void Attack(float time) override;
	void Exit() override;
	void Reload() override;
private:
	void PlayHitSound();
	bool Delay(float delay);

	unsigned int mNumBullets;
	float mShootDuration;
	bool mFirstShoot = true;
	float mTimePassed = 0.0f;
};
