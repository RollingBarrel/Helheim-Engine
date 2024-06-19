#pragma once
#include "MeleeWeapon.h"

class TrailComponent;

class Katana : public MeleeWeapon
{
public:
	Katana();
	~Katana();

	void Enter() override;
	void Attack() override;
	void Exit() override;

private:

	TrailComponent* mTrail = nullptr;
};
