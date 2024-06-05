#pragma once
#include <Script.h>



class Weapon : public Script
{
public:

	Weapon(GameObject* owner) : Script(owner) {}
	virtual void BasicAttack() = 0;
	virtual void SpecialAttack() = 0;

	virtual void Reload() = 0;

protected:

	float mDamage = 0.0f;
	float mAttackRate = 0.0f;
	
};

