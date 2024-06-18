#pragma once
#include "MeleeWeapon.h"

class TrailComponent;

class Hammer : public MeleeWeapon
{
public:
	Hammer();
	~Hammer();

	void Enter() override;
	void Attack() override;
	void Exit() override;

private:

	TrailComponent* mTrail = nullptr;
};
