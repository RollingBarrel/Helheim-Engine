#pragma once
#include "RangeWeapon.h"

class Pistol : public RangeWeapon
{
public:
	Pistol();
	~Pistol();

	void BasicAttack() override;
	void SpecialAttack() override;

	void Reload() override;

};

