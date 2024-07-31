#pragma once
#include "RangeWeapon.h"

class Pistol : public RangeWeapon
{
public:
	Pistol();
	~Pistol() {}

	void Enter() override;
	void Attack(float time) override;
	void Reload();

private:
	void PlayHitSound();

};

