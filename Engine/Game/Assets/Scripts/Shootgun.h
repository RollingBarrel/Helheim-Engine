#pragma once
#include "RangeWeapon.h"

class Shootgun : public RangeWeapon
{
public:
	Shootgun();
	~Shootgun();
	void Enter() override;
	void Attack() override;
	void Exit() override;
	void Reload() override;
};
