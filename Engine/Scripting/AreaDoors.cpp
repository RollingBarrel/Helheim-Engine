#include "AreaDoors.h"
#include "ScriptComponent.h"
#include "BoxColliderComponent.h"
#include "AnimationComponent.h"
#include "GameObject.h"
#include "BattleArea.h"
#include"MeshRendererComponent.h"
#include"ResourceMaterial.h"
#include "GameManager.h"
#include "AudioManager.h"
#include "HudController.h"


CREATE(AreaDoors)
{
	CLASS(owner);
	SEPARATOR("DOOR ENTER");
	MEMBER(MemberType::GAMEOBJECT, mDoorEnter);
	MEMBER(MemberType::GAMEOBJECT, mDoorEnterEmiBorder);
	MEMBER(MemberType::GAMEOBJECT, mDoorEnterEmiTop);
	SEPARATOR("DOOR EXIT");
	MEMBER(MemberType::GAMEOBJECT, mDoorExit);
	MEMBER(MemberType::GAMEOBJECT, mDoorExitEmiBorder);
	MEMBER(MemberType::GAMEOBJECT, mDoorExitEmiTop);
	SEPARATOR("ELEVATOR");
	MEMBER(MemberType::GAMEOBJECT, mElevator);
	SEPARATOR("TUTORIAL");
	MEMBER(MemberType::BOOL, mIsStartingArea);
	END_CREATE;
}

AreaDoors::AreaDoors(GameObject* owner) : Script(owner)
{
}

void AreaDoors::Start()
{

	mCollider = static_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
	if (mCollider)
	{
		mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&AreaDoors::OnCollisionEnter, this, std::placeholders::_1)));
	}
	if (mDoorEnterEmiBorder)
	{
		mBorderRenderEnter = static_cast<MeshRendererComponent*>(mDoorEnterEmiBorder->GetComponent(ComponentType::MESHRENDERER));
	}
	if (mDoorEnterEmiTop)
	{
		mTopRenderEnter = static_cast<MeshRendererComponent*>(mDoorEnterEmiTop->GetComponent(ComponentType::MESHRENDERER));
	}
	if (mDoorExitEmiBorder)
	{
		mBorderRenderExit = static_cast<MeshRendererComponent*>(mDoorExitEmiBorder->GetComponent(ComponentType::MESHRENDERER));
	}
	if (mDoorExitEmiTop)
	{
		mTopRenderExit = static_cast<MeshRendererComponent*>(mDoorExitEmiTop->GetComponent(ComponentType::MESHRENDERER));
	}
	CloseDoors(false);
}

void AreaDoors::Update()
{
}
void AreaDoors::CloseDoors(bool close)
{
	std::string trigger = (close) ? "tClose" : "tOpen";
	if (close)
	{
		GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::DOOR);
		if (mBorderRenderEnter) mBorderRenderEnter->SetEmissiveColor(mClosedColor);
		if (mTopRenderEnter) mTopRenderEnter->SetEmissiveColor(mClosedColor);
		if (mBorderRenderExit) mBorderRenderExit->SetEmissiveColor(mClosedColor);
		if (mTopRenderExit) mTopRenderExit->SetEmissiveColor(mClosedColor);
		
	}
	else
	{
		if (mDoorEnterEmiBorder)
		{
			if (mBorderRenderEnter) mBorderRenderEnter->SetEmissiveColor(mOpenColor);
			if (mTopRenderEnter) mTopRenderEnter->SetEmissiveColor(mOpenColor);
			if (mBorderRenderExit) mBorderRenderExit->SetEmissiveColor(mOpenColor);
			if (mTopRenderExit) mTopRenderExit->SetEmissiveColor(mOpenColor);
		}
	}

	if (mDoorEnter)
	{
		GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::DOOR);
		if (close)
		{

		}
		AnimationComponent* doorAnimation1 = static_cast<AnimationComponent*>(mDoorEnter->GetComponent(ComponentType::ANIMATION));
		if (doorAnimation1)
		{
			doorAnimation1->SetIsPlaying(true);
			doorAnimation1->SendTrigger(trigger, 0.6f);

		}

		BoxColliderComponent* door1Collider = static_cast<BoxColliderComponent*>(mDoorEnter->GetComponent(ComponentType::BOXCOLLIDER));
		if (door1Collider)
		{
			door1Collider->SetEnable(close);
		}
	}
	if (mDoorExit)
	{
		GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::DOOR);

		AnimationComponent* doorAnimation2 = static_cast<AnimationComponent*>(mDoorExit->GetComponent(ComponentType::ANIMATION));
		if (doorAnimation2)
		{
			doorAnimation2->SetIsPlaying(true);
			doorAnimation2->SendTrigger(trigger, 0.6f);
		}

		BoxColliderComponent* door2Collider = static_cast<BoxColliderComponent*>(mDoorExit->GetComponent(ComponentType::BOXCOLLIDER));
		if (door2Collider)
		{
			door2Collider->SetEnable(close);
		}
	}
	if (mElevator)
	{
		mElevator->GetComponent(ComponentType::SCRIPT)->SetEnable(!close);

		if (!close)
		{
			static_cast<AnimationComponent*>((mElevator->GetChildren()[0])->GetComponentInChildren(ComponentType::ANIMATION))->SetAnimSpeed(1.0f);
			static_cast<AnimationComponent*>((mElevator->GetChildren()[0])->GetComponentInChildren(ComponentType::ANIMATION))->SetIsPlaying(true);
		}
		else
		{		
			static_cast<AnimationComponent*>((mElevator->GetChildren()[0])->GetComponentInChildren(ComponentType::ANIMATION))->SetAnimSpeed(-1.0f);
			static_cast<AnimationComponent*>((mElevator->GetChildren()[0])->GetComponentInChildren(ComponentType::ANIMATION))->SetIsPlaying(true);
		}
	}
}
void AreaDoors::OnCollisionEnter(CollisionData* collisionData)
{
	if (collisionData->collidedWith->GetTag().compare("Player") == 0 && !mHasEntered)
	{
		mHasEntered = true;
		CloseDoors(true);
	}
}