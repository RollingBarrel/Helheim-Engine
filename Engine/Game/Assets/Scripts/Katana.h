#pragma once
#include "MeleeWeapon.h"

class TrailComponent;

class Katana : public MeleeWeapon
{
public:
	Katana();
	~Katana();

	void Enter() override;
	void Attack(float time) override;
	void Exit() override;

private:
	void PlayHitSound();
	TrailComponent* mTrail = nullptr;
};
