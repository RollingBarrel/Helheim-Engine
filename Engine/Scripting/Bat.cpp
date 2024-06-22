#include "Bat.h"

Bat::Bat(BoxColliderComponent* collider) : MeleeWeapon(collider)
{
    mAttackDuration = 3.0f;
    mDamage = 2.0f;
    mComboStep = 0;
    mCombo1st = 1.0f;
    mCombo2nd = 2.0f;
    mComboEnd = 3.0f;

}

Bat::~Bat()
{
}



