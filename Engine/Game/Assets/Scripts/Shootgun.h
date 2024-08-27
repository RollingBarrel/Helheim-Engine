#pragma once
#include "RangeWeapon.h"

class Shootgun : public RangeWeapon
{
public:
	Shootgun();
	~Shootgun() {}
	void Enter() override;
	void Attack(float time) override;

private:
	void PlayHitSound();
};
