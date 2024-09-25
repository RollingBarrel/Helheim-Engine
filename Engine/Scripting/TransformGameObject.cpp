#include "TransformGameObject.h"
#include "Application.h"
#include "GameObject.h"
#include "GameManager.h"

#include "ScriptComponent.h"

CREATE(TransformGameObject)
{
	CLASS(owner);
	MEMBER(MemberType::BOOL, mTransformPosition);
	MEMBER(MemberType::BOOL, mTransformRotation);
	MEMBER(MemberType::BOOL, mTransformScale);
	MEMBER(MemberType::FLOAT3, mPosition);
	MEMBER(MemberType::FLOAT3, mRotation);
	MEMBER(MemberType::FLOAT3, mScale);
	MEMBER(MemberType::FLOAT, mDuration);
	END_CREATE;
}

TransformGameObject::TransformGameObject(GameObject* owner) : Script(owner) {}

void TransformGameObject::Start()
{
	mTime = 0;
	float3 actualPosition = mGameObject->GetLocalPosition();
	mPositionOffset = mPosition - actualPosition;
}

void TransformGameObject::Update()
{
	if (mTime < mDuration)
	{
		float mDeltaTime = App->GetDt();
		mTime += mDeltaTime;
		LOG("Timer %f", mTime)
		if (mTransformPosition)
		{
			float3 actualPosition = mGameObject->GetLocalPosition();
			mGameObject->SetLocalPosition(actualPosition + mPositionOffset * mDuration * mDeltaTime);
		}
		if (mTransformRotation)
		{
			float3 actualRotation = mGameObject->GetLocalRotation().ToEulerXYZ();
			mGameObject->SetLocalRotation(actualRotation + mRotationOffset * mDuration * mDeltaTime);
		}
		if (mTransformScale)
		{
			float3 actualScale = mGameObject->GetLocalScale();
			mGameObject->SetLocalPosition(actualScale + mScaleOffset * mDuration * mDeltaTime);
		}
	}
	else
	{
		mTime = 0;
		mGameObject->SetEnabled(false);
	}
}
