#include "Bullet.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"


CREATE(Bullet)
{
	CLASS(owner);

	END_CREATE;
}

Bullet::Bullet(GameObject* owner) : Script(owner) {}


void Bullet::Start()
{

}

void Bullet::Update()
{
	Move();
	//TODO:check if the bullet is out of range

}

void Bullet::Move()
{
	mGameObject->SetPosition(mGameObject->GetPosition() + mGameObject->GetFront()* mSpeed);
}


