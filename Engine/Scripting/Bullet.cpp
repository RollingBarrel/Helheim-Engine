#include "Bullet.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ObjectPool.h"
#include "GameManager.h"
#include "BoxColliderComponent.h"
#include "ParticleSystemComponent.h"

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

void Bullet::Start()
{
	mDirection = GameManager::GetInstance()->GetPlayer()->GetFront();
	mCollider = reinterpret_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
	if (mCollider)
	{
		mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&Bullet::OnCollisionEnter, this, std::placeholders::_1)));
	}
	
	mHitParticles = *(mGameObject->GetChildren().begin());
	if (mHitParticles)
	{
		mHitParticles->SetEnabled(false);
	}

}


void Bullet::Update()
{
	if (!mHasCollided)
	{
		if (mTotalMovement <= mRange)
		{
			mTotalMovement += mGameObject->GetPosition().Distance((mGameObject->GetPosition() + mGameObject->GetFront().Mul(mSpeed)));
			mGameObject->SetPosition(mGameObject->GetPosition() + mDirection * mSpeed);
		}
		else
		{
			App->GetScene()->AddGameObjectToDelete(mGameObject);
		}
	}
	else
	{
		if (Delay(1.0f)) 
		{
		  App->GetScene()->AddGameObjectToDelete(mGameObject);
		}
	}
	
}

void Bullet::OnCollisionEnter(CollisionData* collisionData)
{
	if (collisionData->collidedWith->GetTag().compare("Enemy") == 0) 
	{
		if (mHitParticles)
		{
			mHitParticles->SetEnabled(true);
		}
		mHasCollided = true;
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