#include "Bullet.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ObjectPool.h"

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
    
    if (Delay(0.5f)) {
        objectPool->ReturnToPool(this->mGameObject);
    }
}

bool Bullet::Delay(float delay)
{
    mTimePassed += App->GetDt();

    if (mTimePassed >= delay)
    {
        mTimePassed = 0;
        return true;
    }
    else return false;
}

void Bullet::Move()
{
	mGameObject->SetPosition(mGameObject->GetPosition() + mGameObject->GetFront()* mSpeed);
}


