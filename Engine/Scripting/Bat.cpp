#include "Bat.h"

Bat::Bat(BoxColliderComponent* collider, TrailComponent* trail) : MeleeWeapon(collider, trail)
{
    mDamage = 4.0f;

}

Bat::~Bat()
{
}

void Bat::PlayHitSound()
{
}

void Bat::ApplySpecialEffects(GameObject* enemy)
{
    //LOG("apply special effects bat");
}


