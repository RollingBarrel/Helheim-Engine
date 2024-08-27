#pragma once
#include "RangeWeapon.h"
#include "TimerScript.h"
class Machinegun : public RangeWeapon
{
public:
	Machinegun();
	~Machinegun() {}

	void Enter() override;
	void Attack(float time) override;
	void Exit() override;
private:
	void PlayHitSound();

	unsigned int mNumBullets;
	float mShootDuration;
	bool mFirstShoot = true;
	float mTimePassed = 0.0f;

	TimerScript mShootTimer;

};
