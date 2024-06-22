#include "Katana.h"
#include "Application.h"

Katana::Katana(BoxColliderComponent* collider) : MeleeWeapon(collider)
{
}

Katana::~Katana()
{
}

void Katana::Enter()
{
}

void Katana::Attack(float time)
{
	LOG("Katana Attack");
}

void Katana::Exit()
{
}
