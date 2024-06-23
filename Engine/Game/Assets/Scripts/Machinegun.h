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
};
