#include "SecurityCameraFollower.h"
#include "Application.h"
#include "GameObject.h"

#include "SpotLightComponent.h"
#include "ScriptComponent.h"
#include "BoxColliderComponent.h"

CREATE(SecurityCameraFollower)
{
	CLASS(owner);
	MEMBER(MemberType::FLOAT, mMaxDistance);
	MEMBER(MemberType::FLOAT, mTurningLightOnTime);
	MEMBER(MemberType::FLOAT, mMaxLightIntesity);
	MEMBER(MemberType::FLOAT, mMaxLightRange);
	END_CREATE;
}

SecurityCameraFollower::SecurityCameraFollower(GameObject* owner) : Script(owner) {}

void SecurityCameraFollower::Start()
{
	mCameraLight = static_cast<SpotLightComponent*>(mGameObject->GetComponentInChildren(ComponentType::SPOTLIGHT));
	if (mCameraLight)
	{
		mCameraLight->SetEnable(false);
	}
	mCollider = static_cast<BoxColliderComponent*>(mGameObject->GetComponentInParent(ComponentType::BOXCOLLIDER));
	if (mCollider)
	{
		mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&SecurityCameraFollower::OnCollisionEnter, this, std::placeholders::_1)));
		//mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_EXIT, new std::function<void(CollisionData*)>(std::bind(&SecurityCameraFollower::OnCollisionExit, this, std::placeholders::_1)));
		mCollider->SetColliderType(ColliderType::STATIC);
	}
}

void SecurityCameraFollower::Update()
{
	if (mTarget)
	{
		//Light turns on gradually
		float timeLeftTurningLightOn = mTurningLightOnTime - mTurningLightOnTimer;

		if (timeLeftTurningLightOn <= 0 || mTurningLightOnTime == 0)
		{
			mCameraLight->SetIntensity(mMaxLightIntesity);
			mCameraLight->SetRange(mMaxLightRange);
		}
		else
		{
			mCameraLight->SetIntensity(mMaxLightIntesity * (1 - timeLeftTurningLightOn / mTurningLightOnTime));
			mCameraLight->SetRange(mMaxLightRange * (1 - timeLeftTurningLightOn / mTurningLightOnTime));

			mTurningLightOnTimer += App->GetDt();
		}

		//Rotates security camera to player
		float3 playerFacePosition = mTarget->GetWorldPosition() + float3(0, 0.5f, 0);
		mGameObject->LookAt(mGameObject->GetWorldPosition()+mGameObject->GetWorldPosition()-playerFacePosition);

		//Workaround since OnCollisionExit is not implemented
		if (mTarget->GetWorldPosition().DistanceSq(mGameObject->GetWorldPosition()) > mMaxDistance * mMaxDistance)
		{
			mTarget = nullptr;
			if (mCameraLight) 
			{
				mCameraLight->SetEnable(false);
			}
		}
	}
}

void SecurityCameraFollower::OnCollisionEnter(CollisionData* collisionData)
{
	GameObject* CollisionGO = collisionData->collidedWith;

	if (CollisionGO->GetTag() == "Player" && mTarget == nullptr)
	{
		mTarget = CollisionGO;
		if (mCameraLight)
		{
			mCameraLight->SetEnable(true);
			mCameraLight->SetIntensity(0);
			mCameraLight->SetRange(0);

			mTurningLightOnTimer = 0;
		}
	}
}

//void SecurityCameraFollower::OnCollisionExit(CollisionData* collisionData)
//{
//	GameObject* CollisionGO = collisionData->collidedWith;
//
//	if (CollisionGO->GetTag() == "Player")
//	{
//		mTarget = nullptr;
//		if (mCameraLight) 
//		{
//			mCameraLight->SetEnable(false);
//		}
//	}
//}