#include "MovingObject.h"
#include "Application.h"
#include "GameObject.h"
#include "GameManager.h"

#include "ScriptComponent.h"

CREATE(MovingObject)
{
	CLASS(owner);
	MEMBER(MemberType::FLOAT, mSpeed);
	MEMBER(MemberType::FLOAT3, mDirection);
	MEMBER(MemberType::BOOL, mLoopsPath);
	MEMBER(MemberType::FLOAT, mDistance);
	END_CREATE;
}

MovingObject::MovingObject(GameObject* owner) : Script(owner) {}

void MovingObject::Start()
{
	mStartingPoint = mGameObject->GetWorldPosition();
	mDirection.Normalize();

	mCurrentDistance = 0.0f;
}

void MovingObject::Update()
{
	if (mCurrentDistance <= mDistance && mCurrentDistance >= 0)
	{
		if (mGoingForward)
		{
			mCurrentDistance += App->GetDt() * mSpeed;
		}
		else 
		{
			mCurrentDistance -= App->GetDt() * mSpeed;
		}
		
	}
	else 
	{
		if (mLoopsPath)
		{
			if (mGoingForward)
			{
				mGoingForward = false;
				mCurrentDistance = mDistance;
			}
			else
			{
				mGoingForward = true;
				mCurrentDistance = 0.0f;
			}
		}
		else 
		{
			mCurrentDistance = 0.0f;
		}
	}
	
	mGameObject->SetWorldPosition(mStartingPoint + mCurrentDistance*mDirection);
}
