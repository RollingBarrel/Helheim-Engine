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
		mCameraLight->SetIntensity(0.0f);
		mCameraLight->SetRange(0.0f);
	}
	mCollider = static_cast<BoxColliderComponent*>(mGameObject->GetComponentInParent(ComponentType::BOXCOLLIDER));
	if (mCollider)
	{
		mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&SecurityCameraFollower::OnCollisionEnter, this, std::placeholders::_1)));
		mCollider->SetColliderType(ColliderType::STATIC);
	}
	
	mInitialFront = mGameObject->GetFront().Normalized();

	mLookingAtLocation = mGameObject->GetWorldPosition() - (float3(0.0f,2.0f,0.0f) + mInitialFront);
	mGameObject->LookAt(mGameObject->GetWorldPosition() + mGameObject->GetWorldPosition() - mLookingAtLocation);
	
}

void SecurityCameraFollower::Update()
{
	if (mTarget)
	{
		//Light turns on/off gradually
		float percentageTurningOn = 1.0f - (mTurningOnOffTime - mTurningLightOnTimer) / mTurningOnOffTime;

		//Fading in of the light
		if (mTurningOnOffTime <= mTurningLightOnTimer || mTurningOnOffTime == 0.0f)
		{
			mCameraLight->SetIntensity(mMaxLightIntesity);
			mCameraLight->SetRange(mMaxLightRange);
		}
		else if(!mOutOfReach)
		{
			mCameraLight->SetIntensity(mMaxLightIntesity * percentageTurningOn);
			mCameraLight->SetRange(mMaxLightRange * percentageTurningOn);

			mTurningLightOnTimer += App->GetDt();
		}

		//Rotates security camera to player
		float3 playerPosition = mTarget->GetWorldPosition() + float3(0.0f, 0.5f, 0.0f);
		float3 playerDir = (playerPosition - mGameObject->GetWorldPosition()).Normalized();
		float3 rotationDirection = playerPosition - mLookingAtLocation;

		if (!mOutOfReach && rotationDirection.LengthSq() > 0.01f)
		{
			mLookingAtLocation += rotationDirection * App->GetDt() * mSpeed;
			mGameObject->LookAt(mGameObject->GetWorldPosition() + mGameObject->GetWorldPosition() - mLookingAtLocation);
		}

		//Workaround since OnCollisionExit is not implemented
		if (mTarget->GetWorldPosition().DistanceSq(mGameObject->GetWorldPosition()) > mMaxDistance * mMaxDistance || mInitialFront.Dot(-playerDir) <= 0)
		{
			mOutOfReach = true;
		}

		//Fading out of the light
		if (mOutOfReach)
		{
			mCameraLight->SetIntensity(mMaxLightIntesity * percentageTurningOn);
			mCameraLight->SetRange(mMaxLightRange * percentageTurningOn);

			mTurningLightOnTimer -= App->GetDt();
			if (mTurningLightOnTimer <= 0.0f)
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
