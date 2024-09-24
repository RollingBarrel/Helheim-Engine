#include "Rat.h"
#include "Application.h"
#include "ModuleDetourNavigation.h"

#include "GameObject.h"
#include "AIAGentComponent.h"
#include "BoxColliderComponent.h"

#include "GameManager.h"


CREATE(Rat)
{
    CLASS(owner);
    END_CREATE;
}


void Rat::Start()
{
	Enemy::Start();

	mSpeed = 5.0f;
	mFleeRadius = 3.0f;

	mCollider = static_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
	if (mCollider)
	{
		mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&Rat::OnCollisionEnter, this, std::placeholders::_1)));
	}

}

void Rat::Update()
{
    Enemy::Update(); 
}

void Rat::Idle()
{
	if (IsPlayerInRange(mFleeRadius))
	{
		if (mAiAgentComponent) mAiAgentComponent->StartCrowdNavigation();
		mCurrentState = EnemyState::FLEE;
	}
}

void Rat::Flee()
{
	PlayStepAudio();
	if (mAiAgentComponent)
	{
		float distance = mGameObject->GetWorldPosition().Distance(mPlayer->GetWorldPosition());
		float3 newDir = ((mGameObject->GetWorldPosition() - mPlayer->GetWorldPosition()).Normalized() + mEnemyCollisionDirection).Normalized();
		float3 newPos = mGameObject->GetWorldPosition() + newDir * mSpeed;
		mAiAgentComponent->SetNavigationPath(App->GetNavigation()->FindNearestPoint(newPos, float3(1.0f)));
		mGameObject->LookAt(mGameObject->GetWorldPosition() + mAiAgentComponent->GetDirection());
	}

	if (!IsPlayerInRange(mFleeRadius))
	{
		mCurrentState = EnemyState::IDLE;
	}
}


void Rat::OnCollisionEnter(CollisionData* collisionData)
{
	if (collisionData->collidedWith->GetTag() == "Door" || collisionData->collidedWith->GetTag() == "Bridge")
	{
		mEnemyCollisionDirection = collisionData->collisionNormal;
	}
}
