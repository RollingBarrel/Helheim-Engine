#include "RayCastBullet.h"
#include "Application.h"


#include "GameObject.h"
#include "TrailComponent.h"
#include "ParticleSystemComponent.h"

CREATE(RayCastBullet)
{
	CLASS(owner);
	END_CREATE;
}

void RayCastBullet::Update()
{
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
		}
	}
	else
	{
		if (mHit && mHitParticles)
		{
			mHitParticles->SetEnable(true);
		}

		if (mHitTimer.Delay(mBulletTrail->GetLifeTime()))
		{
			mGameObject->SetEnabled(false);
		}
	}
}

void RayCastBullet::Init(const float3& startposition, const float3& endPosition, float speed, float size, bool hit, ColorGradient* gradient)
{
	mHit = hit;
	mHitPoint = endPosition;
	mDirection = (endPosition - startposition).Normalized();
	mSpeed = speed;

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
