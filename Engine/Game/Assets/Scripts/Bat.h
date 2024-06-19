#pragma once
#include "MeleeWeapon.h"

class TrailComponent;

class Bat : public MeleeWeapon
{
public:
	Bat();
	~Bat();

	void Enter() override;
	void Attack() override;
	void Exit() override;

private:

	TrailComponent* mTrail = nullptr;
};
