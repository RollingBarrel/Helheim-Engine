#pragma once
#include "RangeWeapon.h"

class Machinegun : public RangeWeapon
{
public:
	Machinegun();
	~Machinegun();
	void Enter() override;
	void Attack() override;
	void Exit() override;
	void Reload() override;
};
