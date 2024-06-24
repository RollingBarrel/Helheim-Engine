#include "Hammer.h"
#include "Application.h"

Hammer::Hammer(BoxColliderComponent* collider, TrailComponent* trail) : MeleeWeapon(collider, trail)
{
    mDamage = mHeavyDamage;
    mEnergyCost = mHeavyEnergyCost;
    mCooldownMultiplier = mHeavyCooldownMultiplier;
}

Hammer::~Hammer()
{
}

void Hammer::PlayHitSound()
{
}

void Hammer::ApplySpecialEffects(GameObject* enemy)
{
}

