#include "Bat.h"

Bat::Bat(BoxColliderComponent* collider, TrailComponent* trail) : MeleeWeapon(collider, trail)
{
    mAttackDuration = 3.0f;
    mDamage = 4.0f;
    mComboStep = 0;
    mCombo1st = 1.0f;
    mCombo2nd = 1.0f;
    mComboEnd = 1.0f;

}

Bat::~Bat()
{
}

void Bat::PlayHitSound()
{
}



