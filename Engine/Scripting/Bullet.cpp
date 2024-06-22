#include "Bullet.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ObjectPool.h"
#include "GameManager.h"
#include "BoxColliderComponent.h"
#include "ParticleSystemComponent.h"
#include <TrailComponent.h>

CREATE(Bullet)
{
	CLASS(owner);
	END_CREATE;
}

Bullet::Bullet(GameObject* owner) : Script(owner)
{
}

Bullet::~Bullet()
{
}

void Bullet::Start()
{
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
			mGameObject->SetEnabled(false);
		}
	}
	else
	{
		if (Delay(1.0f)) 
		{
			mGameObject->SetEnabled(false);
		}
	}
}

void Bullet::Init(const float3& position, const float3& direction, float speed, float size,  ColorGradient* gradient)
{
	mTotalMovement = 0;

	mGameObject->SetPosition(position);
	mGameObject->SetScale(float3(size, size, size));
	mDirection = direction;
	mSpeed = speed;

	

	mHitParticles = *(mGameObject->GetChildren().begin());
	if (mHitParticles)
	{
		mHitParticles->SetEnabled(true);
		if (gradient)
		{
			reinterpret_cast<TrailComponent*>(mHitParticles->GetComponent(ComponentType::TRAIL))->SetColorGradient(*gradient);
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