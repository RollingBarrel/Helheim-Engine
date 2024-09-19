#include "SecurityCameraFollower.h"
#include "Application.h"
#include "GameObject.h"

#include "Component.h"
#include "ScriptComponent.h"
#include "BoxColliderComponent.h"

CREATE(SecurityCameraFollower)
{
	CLASS(owner);
	END_CREATE;
}

SecurityCameraFollower::SecurityCameraFollower(GameObject* owner) : Script(owner) {}

void SecurityCameraFollower::Start()
{
	mCameraLight = mGameObject->GetComponentInChildren(ComponentType::SPOTLIGHT);
	if (mCameraLight)
	{
		mCameraLight->SetEnable(false);
	}
	mCollider = static_cast<BoxColliderComponent*>(mGameObject->GetComponentInParent(ComponentType::BOXCOLLIDER));
	if (mCollider)
	{
		mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&SecurityCameraFollower::OnCollisionEnter, this, std::placeholders::_1)));
		mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_EXIT, new std::function<void(CollisionData*)>(std::bind(&SecurityCameraFollower::OnCollisionExit, this, std::placeholders::_1)));
		mCollider->SetColliderType(ColliderType::STATIC);
	}
}

void SecurityCameraFollower::Update()
{
	if (mTarget)
	{
		float3 playerFacePos = mTarget->GetWorldPosition() + float3(0, 1, 0);
		mGameObject->LookAt(mGameObject->GetWorldPosition()+mGameObject->GetWorldPosition()-playerFacePos);
		//mGameObject->SetLocalRotation(mGameObject->GetLocalEulerAngles()-float3(mGameObject->GetLocalEulerAngles().x,0,0));
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
		}
	}
}

void SecurityCameraFollower::OnCollisionExit(CollisionData* collisionData)
{
	GameObject* CollisionGO = collisionData->collidedWith;

	if (CollisionGO->GetTag() == "Player")
	{
		mTarget = nullptr;
		if (mCameraLight) 
		{
			mCameraLight->SetEnable(false);
		}
	}
}