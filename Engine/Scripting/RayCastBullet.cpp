#include "RayCastBullet.h"
#include "Application.h"

#include "GameObject.h"
#include "TrailComponent.h"
#include "ParticleSystemComponent.h"
#include "ScriptComponent.h"

#include "GameManager.h"
#include "PlayerController.h"
#include "Enemy.h"

CREATE(RayCastBullet)
{
	CLASS(owner);
	END_CREATE;
}

void RayCastBullet::Update()
{
	if (mFirstFrame)
	{
		mFirstFrame = false;
		return;
	}


	if ((mBulletTrail->GetOwner()->GetWorldPosition() - mHitPoint).Dot(mDirection) < 0)
	{
		float3 newPosition = mBulletTrail->GetOwner()->GetWorldPosition() + mDirection * mSpeed * App->GetDt();

		if ((newPosition - mHitPoint).Dot(mDirection) < 0)
		{
			mBulletTrail->GetOwner()->SetWorldPosition(newPosition);
		}
		else
		{
			mBulletTrail->GetOwner()->SetWorldPosition(mHitPoint + mDirection * 1.01f);

			if (mHit.IsValid())
			{
				if (mHit.mGameObject->GetTag().compare("Enemy") == 0)
				{
					Enemy* enemy = static_cast<Enemy*>(((ScriptComponent*)mHit.mGameObject->GetComponentInParent(ComponentType::SCRIPT))->GetScriptInstance());
					if (enemy)
					{
						enemy->TakeDamage(mDamage * GameManager::GetInstance()->GetPlayerController()->GetDamageModifier());
					}
				}
			}
		}
	}
	else
	{
		if (mHit.IsValid() && mHitParticles)
		{
			mHitParticles->SetEnable(true);
		}

		if (mHitTimer.Delay(mBulletTrail->GetLifeTime()))
		{
			mGameObject->SetEnabled(false);
		}
	}
}

void RayCastBullet::Init(const float3& startposition, Hit& hit, float damage, float speed, float size, const ColorGradient* gradient)
{
	mHit = hit;
	Init(startposition, hit.mHitPoint, damage, speed, size, gradient);
}

void RayCastBullet::Init(const float3& startposition, const float3& endPosition, float damage, float speed, float size, const ColorGradient* gradient)
{
	mFirstFrame = true;

	mHitPoint = endPosition;
	mDirection = (endPosition - startposition).Normalized();
	mSpeed = speed;
	mDamage = damage;

	mGameObject->SetEnabled(true);

	mHitParticles = static_cast<ParticleSystemComponent*>(mGameObject->GetComponentInChildren(ComponentType::PARTICLESYSTEM));
	mBulletTrail = static_cast<TrailComponent*>(mGameObject->GetComponentInChildren(ComponentType::TRAIL));
	if (mBulletTrail)
	{
		mBulletTrail->GetOwner()->SetWorldPosition(startposition);
		mBulletTrail->SetEnable(true);
		if (gradient)
		{
			mBulletTrail->SetColorGradient(*gradient);
		}
	}
	if (mHitParticles)
	{
		mHitParticles->GetOwner()->SetWorldPosition(mHitPoint);
		mHitParticles->SetEnable(false);
	}
}
