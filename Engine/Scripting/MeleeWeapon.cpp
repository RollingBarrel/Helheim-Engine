#include "MeleeWeapon.h"
#include "TrailComponent.h"

MeleeWeapon::MeleeWeapon() : Weapon()
{
	mType = WeaponType::MELEE;
}

MeleeWeapon::~MeleeWeapon()
{
}
