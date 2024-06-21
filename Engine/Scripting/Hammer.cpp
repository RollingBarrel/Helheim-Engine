#include "Hammer.h"

Hammer::Hammer(BoxColliderComponent* collider) : MeleeWeapon(collider)
{
    mDamage = 7.0f;
}

Hammer::~Hammer()
{
}

void Hammer::Enter()
{
}

void Hammer::Attack(float time)
{

}

void Hammer::Exit()
{
}
