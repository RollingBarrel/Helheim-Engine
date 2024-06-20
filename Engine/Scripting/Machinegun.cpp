#include "Machinegun.h"

Machinegun::Machinegun()
{
	mCurrentAmmo = 32;
	mMaxAmmo = 32;

	mDamage = 0.4f;
	mAttackTime = 0.1f;
}

Machinegun::~Machinegun()
{
}

void Machinegun::Enter()
{
}

void Machinegun::Attack(float time)
{
}

void Machinegun::Exit()
{
}

void Machinegun::Reload() 
{
}
