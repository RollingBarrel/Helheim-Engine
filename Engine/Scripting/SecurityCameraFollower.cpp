#include "SecurityCameraFollower.h"
#include "Application.h"
#include "GameObject.h"

#include "SpotLightComponent.h"
#include "ScriptComponent.h"
#include "BoxColliderComponent.h"

CREATE(SecurityCameraFollower)
{
	CLASS(owner);
	MEMBER(MemberType::FLOAT, mSpeed);
	MEMBER(MemberType::FLOAT, mMaxDistance);
	MEMBER(MemberType::FLOAT, mTurningOnOffTime);
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
		mCameraLight->SetIntensity(0);
		mCameraLight->SetRange(0);
	}
	mCollider = static_cast<BoxColliderComponent*>(mGameObject->GetComponentInParent(ComponentType::BOXCOLLIDER));
	if (mCollider)
	{
		mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&SecurityCameraFollower::OnCollisionEnter, this, std::placeholders::_1)));
		//mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_EXIT, new std::function<void(CollisionData*)>(std::bind(&SecurityCameraFollower::OnCollisionExit, this, std::placeholders::_1)));
		mCollider->SetColliderType(ColliderType::STATIC);
	}
	
	mLookingAtLocation = mGameObject->GetWorldPosition() - float3(0.0f,5.0f,0.0f);
	
}

void SecurityCameraFollower::Update()
{
	if (mTarget)
	{
		//Light turns on/off gradually
		float timeLeftTurningLightOn = mTurningOnOffTime - mTurningLightOnTimer;

		//Fading in of the light
		if (timeLeftTurningLightOn <= 0 || mTurningOnOffTime == 0)
		{
			mCameraLight->SetIntensity(mMaxLightIntesity);
			mCameraLight->SetRange(mMaxLightRange);
		}
		else if(!mOutOfReach)
		{
			mCameraLight->SetIntensity(mMaxLightIntesity * (1 - timeLeftTurningLightOn / mTurningOnOffTime));
			mCameraLight->SetRange(mMaxLightRange * (1 - timeLeftTurningLightOn / mTurningOnOffTime));

			mTurningLightOnTimer += App->GetDt();
		}

		//Rotates security camera to player
		float3 playerPosition = mTarget->GetWorldPosition() + float3(0, 0.5f, 0);
		float3 rotationDirection = playerPosition - mLookingAtLocation;
		if (rotationDirection.LengthSq() > 0.01)
		{
			mLookingAtLocation += rotationDirection * App->GetDt() * mSpeed;
			mGameObject->LookAt(mGameObject->GetWorldPosition() + mGameObject->GetWorldPosition() - mLookingAtLocation);
		}

		//Workaround since OnCollisionExit is not implemented
		if (mTarget->GetWorldPosition().DistanceSq(mGameObject->GetWorldPosition()) > mMaxDistance * mMaxDistance)
		{
			mOutOfReach = true;
		}

		//Fading out of the light
		if (mCameraLight->IsEnabled() && mOutOfReach)
		{
			mOutOfReach = true;

			mCameraLight->SetIntensity(mMaxLightIntesity * (1 - timeLeftTurningLightOn / mTurningOnOffTime));
			mCameraLight->SetRange(mMaxLightRange * (1 - timeLeftTurningLightOn / mTurningOnOffTime));

			mTurningLightOnTimer -= App->GetDt();
			if (mTurningLightOnTimer <= 0)
			{
				mTarget = nullptr;
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
			mOutOfReach = false;
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