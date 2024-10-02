#include "LinearMovement.h"
#include "Application.h"
#include "GameObject.h"
#include "ScriptComponent.h"
#include "AnimationComponent.h"

#include "MathConstants.h"

CREATE(LinearMovement)
{
	CLASS(owner);
	MEMBER(MemberType::FLOAT3, mTargetPosition);
	MEMBER(MemberType::FLOAT, mSpeed);
	MEMBER(MemberType::FLOAT, mReturnIn);
	MEMBER(MemberType::BOOL, mTeleportBack);
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
	mInitialPosition = mGameObject->GetWorldPosition();
	mCurrentPosition = mInitialPosition;
	mAnimationComponent = static_cast<AnimationComponent*>(mGameObject->GetComponentInChildren(ComponentType::ANIMATION));

	if (mAnimationComponent) mAnimationComponent->SetIsPlaying(true);

	mGameObject->LookAt(mTargetPosition);
}

void LinearMovement::Update()
{
	if (!mReachedTarget) Movement(mTargetPosition, mSpeed);
	else
	{
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
				mGameObject->SetWorldPosition(mInitialPosition);
				mReachedTarget = false;
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
		mHoveringPosition = mCurrentPosition + mGameObject->GetUp().Normalized() * mHoveringHeight * sinf(mHoveringTimer);
		mHoveringTimer += mHoveringFrequency * App->GetDt() / (2 * pi);

		mGameObject->SetWorldPosition(mHoveringPosition);
	}
	else 
	{
		mGameObject->SetWorldPosition(mCurrentPosition);
	}
	
	float dif = target.Distance(mCurrentPosition);
	if (dif < 0.1f) mReachedTarget = true;
}
