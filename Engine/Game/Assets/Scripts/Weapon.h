#pragma once
#include <Script.h>



class Weapon : public Script
{

	virtual void BasicAttack() = 0;
	virtual void SpecialAttack() = 0;

private:

	float mDamage;
	float mAttackRate;
	

};

