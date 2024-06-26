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
}


void Bullet::Update()
{
	if (!mHasCollided)
	{
		if (mTotalMovement <= mRange)
		{
			mTotalMovement += mGameObject->GetWorldPosition().Distance((mGameObject->GetWorldPosition() + mGameObject->GetFront().Mul(mSpeed)));
			mGameObject->SetWorldPosition(mGameObject->GetWorldPosition() + mDirection * mSpeed);
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

	mGameObject->SetWorldPosition(position);
	mGameObject->SetWorldScale(float3(size, size, size));
	mDirection = direction;
	mSpeed = speed;
	mHasCollided = false;

	mGameObject->SetEnabled(true);

	GameObject* firstChild = *(mGameObject->GetChildren().begin());
	if (firstChild)
	{
		mHitParticles = reinterpret_cast<ParticleSystemComponent*>(firstChild->GetComponent(ComponentType::PARTICLESYSTEM));
		mBulletTrail = reinterpret_cast<TrailComponent*>(firstChild->GetComponent(ComponentType::TRAIL));
		if (mBulletTrail)
		{
			mBulletTrail->SetEnable(true);
		}
		if (mHitParticles)
		{
			mHitParticles->SetEnable(false);
		}
		if (gradient)
		{
			mBulletTrail->SetColorGradient(*gradient);
		}
	}

}


void Bullet::OnCollisionEnter(CollisionData* collisionData)
{
	if (collisionData->collidedWith->GetTag().compare("Enemy") == 0 || collisionData->collidedWith->GetTag().compare("Wall") == 0)
	{
		if (mHitParticles)
		{
			mHitParticles->SetEnable(true);
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