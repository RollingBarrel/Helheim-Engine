#pragma once
#include "RangeWeapon.h"

class Pistol : public RangeWeapon
{
public:
	Pistol();
	~Pistol();
	void Enter() override;
	void Attack() override;
	void Exit() override;

	void Reload() override;

};

