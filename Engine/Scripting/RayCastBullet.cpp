#include "RayCastBullet.h"
#include "Application.h"

#include "GameObject.h"
#include "TrailComponent.h"
#include "Trail.h"
#include "ParticleSystemComponent.h"
#include "DecalComponent.h"
#include "ScriptComponent.h"

#include "GameManager.h"
#include "PlayerController.h"
#include "Enemy.h"

CREATE(RayCastBullet)
{
	CLASS(owner);
	MEMBER(MemberType::FLOAT, mDecalLifetime);
	MEMBER(MemberType::FLOAT, mDecalFadingTime);
	END_CREATE;
}

void RayCastBullet::Update()
{
	if (mFirstFrame)
	{
		mFirstFrame = false;
		return;
	}


	if ((mBulletTrail->GetOwner()->GetWorldPosition() - mHitPoint).Dot(mDirection) < 0.0f)
	{
		float3 newPosition = mBulletTrail->GetOwner()->GetWorldPosition() + mDirection * mSpeed * App->GetDt();

		if ((newPosition - mHitPoint).Dot(mDirection) < 0)
		{
			mBulletTrail->GetOwner()->SetWorldPosition(newPosition);
		}
		else
		{
			mBulletTrail->GetOwner()->SetWorldPosition(mHitPoint + mDirection * 0.00001f);

			if (mHit.IsValid())
			{
				if (mHit.mGameObject->GetTag().compare("Enemy") == 0)
				{
					Enemy* enemy = static_cast<Enemy*>(((ScriptComponent*)mHit.mGameObject->GetComponentInParent(ComponentType::SCRIPT))->GetScriptInstance());
					if (enemy)
					{
						enemy->TakeDamage(mDamage * GameManager::GetInstance()->GetPlayerController()->GetDamageModifier());
						GameManager::GetInstance()->HitStop();
					}
				}
				else
				{

					mHoleDecal->GetOwner()->SetEnabled(true);
					//TODO: Set hole decal rotation
				}
			}
		}
	}
	else
	{
		if (mHit.IsValid()) 
		{
			if (mHitParticles)
			{
				mHitParticles->GetOwner()->SetEnabled(true);
			}
			if (mHitTimer.Delay(mDecalLifetime) && !mFadeDecal)
			{
				mFadeDecal = true;
			}
		}
		else 
		{
			if (mHitTimer.Delay(mBulletTrail->GetTrail()->GetLifeTime()))
			{
				mHit = Hit();
				mGameObject->SetEnabled(false);
			}
		}

		if (mFadeDecal) 
		{
			if (mDecalFadingTime > 0) 
			{
				mHoleDecal->SetFadeFactor( mHoleDecal->GetFadeFactor() - App->GetDt()/mDecalFadingTime );
			}
			if (mHoleDecal->GetFadeFactor() == 0) 
			{
				//Reset timer and decal values
				mHit = Hit();
				mHitTimer.Reset();
				mFadeDecal = false;
				mHoleDecal->SetFadeFactor(1.0f);
				mGameObject->SetEnabled(false);
			}
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
	mHoleDecal = static_cast<DecalComponent*>(mGameObject->GetComponentInChildren(ComponentType::DECAL));

	if (mBulletTrail)
	{
		mBulletTrail->GetOwner()->SetWorldPosition(startposition);
		mBulletTrail->SetEnable(true);
		if (gradient)
		{
			mBulletTrail->GetTrail()->SetColorGradient(*gradient);
		}
	}
	if (mHitParticles)
	{
		mHitParticles->GetOwner()->SetWorldPosition(mHitPoint);
		mHitParticles->GetOwner()->SetEnabled(false);
	}
	if (mHoleDecal) 
	{
		mHoleDecal->GetOwner()->SetWorldPosition(mHitPoint);
		mHoleDecal->GetOwner()->SetEnabled(false);
	}
}
