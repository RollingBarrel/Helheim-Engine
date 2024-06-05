#pragma once
#include "RangeWeapon.h"
#include "Macros.h"

GENERATE_BODY(Pistol);
class Pistol : public RangeWeapon
{
	FRIEND(Pistol)
public:
	void Start() override;

	Pistol(GameObject* owner) : RangeWeapon(owner) {}

	void BasicAttack() override;
	void SpecialAttack() override;

	void Reload() override;

};

