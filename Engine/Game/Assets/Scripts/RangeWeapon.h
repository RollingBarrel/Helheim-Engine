#pragma once
#include "Weapon.h"

class GameObject;
class HudController;

class RangeWeapon : public Weapon
{

public:
	RangeWeapon(GameObject* owner) : Weapon(owner) {}

protected:

	GameObject* mShootPoint = nullptr;

	GameObject* mHudControllerGO = nullptr;
	HudController* mHudController = nullptr;

	int mCurrentAmmo = 16;
	int mMaxAmmo = 16;

};

