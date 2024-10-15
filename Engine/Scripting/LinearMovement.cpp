#include "LinearMovement.h"
#include "Application.h"
#include "GameObject.h"
#include "ScriptComponent.h"
#include "AnimationComponent.h"
#include "ParticleSystemComponent.h"

#include "MathConstants.h"

CREATE(LinearMovement)
{
	CLASS(owner);
	MEMBER(MemberType::FLOAT3, mTargetPosition);
	MEMBER(MemberType::FLOAT, mSpeed);
	MEMBER(MemberType::FLOAT, mReturnIn);
	MEMBER(MemberType::BOOL, mTeleportBack);
	MEMBER(MemberType::BOOL, mLocalPosition);
	MEMBER(MemberType::BOOL, mLookAt);
	MEMBER(MemberType::BOOL, mTargetAGO);
	MEMBER(MemberType::GAMEOBJECT, mTargetGO);
	SEPARATOR("Hovering");
	MEMBER(MemberType::BOOL, mHoveringMovement);
	MEMBER(MemberType::FLOAT, mHoveringHeight);
	MEMBER(MemberType::FLOAT, mHoveringFrequency);
	END_CREATE;
}

LinearMovement::LinearMovement(GameObject* owner) : Script (owner)
{

}

void LinearMovement::Start()
{
	if (!mLocalPosition) mInitialPosition = mGameObject->GetWorldPosition();
	else mInitialPosition = mGameObject->GetLocalPosition();
	mCurrentPosition = mInitialPosition;
	mAnimationComponent = static_cast<AnimationComponent*>(mGameObject->GetComponentInChildren(ComponentType::ANIMATION));
	mParticleComponent = reinterpret_cast<ParticleSystemComponent*>(mGameObject->GetComponent(ComponentType::PARTICLESYSTEM));

	if (mAnimationComponent) mAnimationComponent->SetIsPlaying(true);

	if(mLookAt) mGameObject->LookAt(mTargetPosition);
}

void LinearMovement::Update()
{
	if (!mReachedTarget) Movement(mTargetPosition, mSpeed);
	else
	{
		if (mParticleComponent) mParticleComponent->SetEnable(false);
		if (mReturnTimer.Delay(mReturnIn))
		{
			if (!mTeleportBack)
			{
				mTargetPosition = mInitialPosition;
				mInitialPosition = mCurrentPosition;
				mReachedTarget = false;
				mGameObject->LookAt(mTargetPosition);

			}
			else
			{
				mCurrentPosition = mInitialPosition;
				if(!mLocalPosition) mGameObject->SetWorldPosition(mInitialPosition);
				else mGameObject->SetLocalPosition(mInitialPosition);
				mReachedTarget = false;
				mGameObject->SetEnabled(true);
				if (mParticleComponent) mParticleComponent->SetEnable(true);
			}
		}
		mHoveringTimer = 0.0f;
	}
}

void LinearMovement::Movement(float3 target, float speed)
{
	float3 direction = target.Sub(mCurrentPosition).Normalized();
	float3 velocity = direction * speed;
	mCurrentPosition += velocity * App->GetDt();

	if (mHoveringMovement)
	{
		mHoveringPosition = mCurrentPosition + mGameObject->GetUp() * mHoveringHeight * sinf(mHoveringTimer);
		mHoveringTimer += mHoveringFrequency * App->GetDt() / (2.0f * pi);

		mGameObject->SetWorldPosition(mHoveringPosition);
	}
	else 
	{
		mGameObject->SetWorldPosition(mCurrentPosition);
	}
	
	float dif = target.Distance(mCurrentPosition);
	if (dif < 0.1f)
	{
		mReachedTarget = true;
		mCurrentPosition = target;
	}
	
	if (!mHoveringMovement)
	{
		if (!mLocalPosition) mGameObject->SetWorldPosition(mCurrentPosition);
		else mGameObject->SetLocalPosition(mCurrentPosition);
	}
	
	
}
