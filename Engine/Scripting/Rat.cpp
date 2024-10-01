#include "Rat.h"
#include "Application.h"
#include "ModuleDetourNavigation.h"

#include "GameObject.h"
#include "AnimationComponent.h"
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
	mAnimationComponent->SetAnimSpeed(1.0f);
	if (IsPlayerInRange(mFleeRadius))
	{
		if (mAiAgentComponent) mAiAgentComponent->StartCrowdNavigation();
		mCurrentState = EnemyState::FLEE;
	}
}

void Rat::Flee()
{
	mAnimationComponent->SetAnimSpeed(2.5f);
	if (mAiAgentComponent && !mIsFleeing)
	{
		!(mAiAgentComponent->FleeFromTarget(mPlayer->GetWorldPosition()));
		mGameObject->LookAt(mGameObject->GetWorldPosition() + mAiAgentComponent->GetDirection());
	}

	if (!IsPlayerInRange(mFleeRadius))
	{
		mIsFleeing = false;
		mCurrentState = EnemyState::IDLE;
	}
}


void Rat::OnCollisionEnter(CollisionData* collisionData)
{
	if (collisionData->collidedWith->GetTag() == "Door" || collisionData->collidedWith->GetTag() == "Bridge")
	{
		mAiAgentComponent->FleeFromTarget(collisionData->collidedWith->GetWorldPosition());
		mGameObject->LookAt(mGameObject->GetWorldPosition() + mAiAgentComponent->GetDirection());
		mIsFleeing = true;
	}
}
