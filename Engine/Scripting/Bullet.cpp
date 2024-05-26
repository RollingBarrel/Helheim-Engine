#include "Bullet.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ObjectPool.h"

unsigned int Bullet::mNumBullets = 0;

CREATE(Bullet)
{
	CLASS(owner);

	END_CREATE;
}

Bullet::Bullet(GameObject* owner) : Script(owner)
{
	mNumBullets++;
}

Bullet::~Bullet()
{
	mNumBullets--;
}



void Bullet::Update()
{
	if (mTotalMovement <= mRange)
	{
		mTotalMovement += mGameObject->GetPosition().Distance((mGameObject->GetPosition() + mGameObject->GetFront() * mSpeed));
		mGameObject->SetPosition(mGameObject->GetPosition() + mGameObject->GetFront() * mSpeed);
	}
	else
	{
		App->GetScene()->AddGameObjectToDelete(mGameObject);
	}
	
}



