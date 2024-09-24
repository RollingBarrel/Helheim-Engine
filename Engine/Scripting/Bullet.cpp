#include "Bullet.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ObjectPool.h"
#include "GameManager.h"
#include "BoxColliderComponent.h"
#include "ParticleSystemComponent.h"
#include "TrailComponent.h"
#include "Trail.h"
#include "ScriptComponent.h"
#include "PlayerController.h"

CREATE(Bullet)
{
	CLASS(owner);
	MEMBER(MemberType::BOOL, mShooterIsPlayer);

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
			//LOG("TotalMovement, %f", mTotalMovement);

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
		if (mBullet)
		{
			mBullet->SetEnable(false);
		}
		if (mTrialParticles)
		{
			mTrialParticles->SetEnable(false);
		}
		if (mHitParticles)
		{
			mHitParticles->SetEnable(true);
		}
	}
}


void Bullet::OnCollisionEnter(CollisionData* collisionData)
{
	if (!mHasCollided)
	{
		if (mShooterIsPlayer)
		{
			if (collisionData->collidedWith->GetTag().compare("Enemy") == 0)
			{
				//LOG("Collided with Enemy: %s", collisionData->collidedWith->GetName().c_str());
				if(mBullet)
				{ 
					mBullet->SetEnable(false);
				}
				if(mTrialParticles)
				{ 
					mTrialParticles->SetEnable(false);
				}
				if (mHitParticles)
				{
					mHitParticles->SetEnable(true);
				}
				mHasCollided = true;
				return;
			}
		}
		else
		{
			if (collisionData->collidedWith->GetTag().compare("Player") == 0)
			{
				//LOG("Collided with player");
				ScriptComponent* playerScript = static_cast<ScriptComponent*>(GameManager::GetInstance()->GetPlayer()->GetComponent(ComponentType::SCRIPT));
				PlayerController* player = static_cast<PlayerController*>(playerScript->GetScriptInstance());
				player->TakeDamage(mDamage);
				mDamage = 0.0f;
				if (mBullet)
				{
					mBullet->SetEnable(false);
				}
				if (mTrialParticles)
				{
					mTrialParticles->SetEnable(false);
				}
				if (mHitParticles)
				{
					mHitParticles->SetEnable(true);
				}
				GameManager::GetInstance()->HitStop();
				mHasCollided = true;
			}
		}

		if (collisionData->collidedWith->GetTag().compare("Wall") == 0)
		{
			//LOG("Collided with WALL");
			if (mBullet)
			{
				mBullet->SetEnable(false);
			}
			if (mTrialParticles)
			{
				mTrialParticles->SetEnable(false);
			}
			if (mHitParticles)
			{
				mHitParticles->SetEnable(true);
			}
			mHasCollided = true;
			return;
		}
	}
}