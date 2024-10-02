#include "Bullet.h"
#include "Application.h"
#include "GameObject.h"
#include "BoxColliderComponent.h"
#include "ParticleSystemComponent.h"
#include "TrailComponent.h"

#include "GameManager.h"
#include "PlayerController.h"


CREATE(Bullet)
{
	CLASS(owner);
	END_CREATE;
}

void Bullet::Start()
{
	mCollider = static_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
	if (mCollider)
	{
		mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&Bullet::OnCollisionEnter, this, std::placeholders::_1)));
	}
}

void Bullet::Update()
{
	if (GameManager::GetInstance()->IsPaused()) return;

	if (!mHasCollided)
	{
		if (mTotalMovement == 0.0f)
		{
			mTotalMovement += mGameObject->GetWorldPosition().Distance((mGameObject->GetWorldPosition() + mGameObject->GetFront().Mul(mSpeed * App->GetDt())));
		}
		else if (mTotalMovement <= mRange)
		{
			mTotalMovement += mGameObject->GetWorldPosition().Distance((mGameObject->GetWorldPosition() + mGameObject->GetFront().Mul(mSpeed * App->GetDt())));
			mGameObject->SetWorldPosition(mGameObject->GetWorldPosition() + mDirection * mSpeed * App->GetDt());
		}
		else
		{
			mGameObject->SetEnabled(false);
		}
	}
	else
	{
		if (mTimer.Delay(1.0f))
		{
			mGameObject->SetEnabled(false);
		}
	}
}

void Bullet::Init(const float3& position, const float3& direction, float speed, float size, ColorGradient* gradient, float damage, float range)
{
	mTotalMovement = 0.0f;
	mHasCollided = false;

	mGameObject->SetWorldPosition(position);
	mGameObject->SetWorldScale(float3(size, size, size));
	mDirection = direction;
	mSpeed = speed;
	mDamage = damage;
	mRange = range;

	mGameObject->SetEnabled(true);
	mBullet = static_cast<ParticleSystemComponent*>(mGameObject->GetComponent(ComponentType::PARTICLESYSTEM));

	GameObject* firstChild = *(mGameObject->GetChildren().begin());
	if (firstChild)
	{
		mHitParticles = static_cast<ParticleSystemComponent*>(firstChild->GetComponent(ComponentType::PARTICLESYSTEM));
		mTrialParticles = static_cast<TrailComponent*>(firstChild->GetComponent(ComponentType::TRAIL));
		if (mBullet) mBullet->SetEnable(true);
		if (mTrialParticles) mTrialParticles->SetEnable(true);
		if (mHitParticles) mHitParticles->SetEnable(true);
	}
}

void Bullet::OnCollisionEnter(CollisionData* collisionData)
{
	if (!mHasCollided)
	{
		if (collisionData->collidedWith->GetTag().compare("Player") == 0 && !GameManager::GetInstance()->GetPlayerController()->IsPlayerDashing())
		{
			PlayerController* player = GameManager::GetInstance()->GetPlayerController();
			player->TakeDamage(mDamage);
			mDamage = 0.0f;
			if (mBullet) mBullet->SetEnable(false);
			if (mTrialParticles) mTrialParticles->SetEnable(false);
			if (mHitParticles) mHitParticles->SetEnable(true);
			GameManager::GetInstance()->HitStop();
			mHasCollided = true;
		}

		if (collisionData->collidedWith->GetTag().compare("Wall") == 0 || collisionData->collidedWith->GetTag().compare("Door") == 0)
		{
			if (mBullet) mBullet->SetEnable(false);
			if (mTrialParticles) mTrialParticles->SetEnable(false);
			if (mHitParticles)	mHitParticles->SetEnable(true);
			mHasCollided = true;
			return;
		}
	}
}