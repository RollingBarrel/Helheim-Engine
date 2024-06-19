#pragma once
#include "MeleeWeapon.h"

class GameObject;

class Bat : public MeleeWeapon
{
public:
	Bat();
	~Bat();

	void Enter() override;
	void BasicAttack() override;
	void SpecialAttack() override;
	void Exit() override;

private:

	GameObject* mTrail = nullptr;
};
