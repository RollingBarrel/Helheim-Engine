#pragma once
#include "MeleeWeapon.h"

class Bat : public MeleeWeapon
{
public:
	Bat();
	~Bat();

	void BasicAttack() override;
	void SpecialAttack() override;

};
