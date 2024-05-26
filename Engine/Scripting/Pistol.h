#pragma once
#include "RangeWeapon.h"
class Pistol : public RangeWeapon
{

public:

	void BasicAttack() override;
	void SpecialAttack() override;

};

