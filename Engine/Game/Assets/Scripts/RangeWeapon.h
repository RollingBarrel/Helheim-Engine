#pragma once
#include "Weapon.h"

class GameObject;
class HudController;

class RangeWeapon : public Weapon
{

public:
	RangeWeapon(GameObject* owner) : Weapon(owner) {}

	int GetCurrentAmmo() { return mCurrentAmmo; }
	int GetMaxAmmo() { return mMaxAmmo; }

protected:

	GameObject* mShootPoint = nullptr;

	int mCurrentAmmo = 16;
	int mMaxAmmo = 16;

};

