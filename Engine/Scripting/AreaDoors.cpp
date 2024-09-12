#include "AreaDoors.h"
#include "ScriptComponent.h"
#include "BoxColliderComponent.h"
#include "AnimationComponent.h"
#include "GameObject.h"
#include "BattleArea.h"

#include "GameManager.h"
#include "AudioManager.h"
#include "HudController.h"


CREATE(AreaDoors)
{
	CLASS(owner);
	SEPARATOR("DOORS");
	MEMBER(MemberType::GAMEOBJECT, mDoor1);
	MEMBER(MemberType::GAMEOBJECT, mDoor2);

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
}

void AreaDoors::Update()
{
}
void AreaDoors::CloseDoors(bool close)
{
	std::string trigger = (close) ? "tClose" : "tOpen";

	if (mDoor1)
	{
		GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::DOOR, mDoor1->GetWorldPosition());

		AnimationComponent* doorAnimation1 = static_cast<AnimationComponent*>(mDoor1->GetComponent(ComponentType::ANIMATION));
		if (doorAnimation1)
		{
			doorAnimation1->SetIsPlaying(true);
			doorAnimation1->SendTrigger(trigger, 0.6f);

		}

		BoxColliderComponent* door1Collider = static_cast<BoxColliderComponent*>(mDoor1->GetComponent(ComponentType::BOXCOLLIDER));
		if (door1Collider)
		{
			door1Collider->SetEnable(close);
		}
	}
	if (mDoor2)
	{
		GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::DOOR, mDoor2->GetWorldPosition());

		AnimationComponent* doorAnimation2 = static_cast<AnimationComponent*>(mDoor2->GetComponent(ComponentType::ANIMATION));
		if (doorAnimation2)
		{
			doorAnimation2->SetIsPlaying(true);
			doorAnimation2->SendTrigger(trigger, 0.6f);
		}

		BoxColliderComponent* door2Collider = static_cast<BoxColliderComponent*>(mDoor2->GetComponent(ComponentType::BOXCOLLIDER));
		if (door2Collider)
		{
			door2Collider->SetEnable(close);
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