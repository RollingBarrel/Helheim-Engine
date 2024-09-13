#include "LinearMovement.h"
#include "Application.h"
#include "GameObject.h"
#include "ScriptComponent.h"
#include "AnimationComponent.h"
#include "ModuleInput.h"
#include "Keys.h"

#include "MathFunc.h"

CREATE(LinearMovement)
{
	CLASS(owner);
	MEMBER(MemberType::FLOAT3, mTargetPosition);
	MEMBER(MemberType::FLOAT, mSpeed);
	END_CREATE;
}

LinearMovement::LinearMovement(GameObject* owner) : Script (owner)
{

}

void LinearMovement::Start()
{
	mInitialPosition = mGameObject->GetWorldPosition();
	mAnimationComponent = static_cast<AnimationComponent*>(mGameObject->GetComponentInChildren(ComponentType::ANIMATION));
	if (mAnimationComponent)
	{
		mAnimationComponent->SetIsPlaying(true);
	}
	mGameObject->LookAt(mTargetPosition);
}

void LinearMovement::Update()
{
	if (App->GetInput()->GetKey(Keys::Keys_F) == KeyState::KEY_DOWN || mStarted)
	{
		mStarted = true;
	
	if (!mReachedTarget)
	{
		mInitialPosition.x = Lerp(mInitialPosition.x, mTargetPosition.x, App->GetDt() * mSpeed);
		mInitialPosition.z = Lerp(mInitialPosition.z, mTargetPosition.z, App->GetDt() * mSpeed);
		mGameObject->SetWorldPosition(mInitialPosition);
		float dif = mTargetPosition.SumOfElements() - mInitialPosition.SumOfElements();
		if (dif < 1) mReachedTarget = true;
	}
	else
	{
		LOG("REACHED TARGET")
	}}
}
