#include "Rat.h"
#include "Application.h"
#include "ModuleDetourNavigation.h"

#include "GameObject.h"
#include "AnimationComponent.h"
#include "AIAGentComponent.h"
#include "BoxColliderComponent.h"
#include "DecalComponent.h"
#include "ParticleSystemComponent.h"

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
	mFleeToAttackTime = 2.0f;
	mDeathTime = 4.0f;
	mCollider = static_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
	if (mCollider)
	{
		mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&Rat::OnCollisionEnter, this, std::placeholders::_1)));
	}

	mDeathParticles = static_cast<ParticleSystemComponent*>(mGameObject->GetComponentInChildren(ComponentType::PARTICLESYSTEM));
	mDecalComponent = static_cast<DecalComponent*>(mGameObject->GetComponentInChildren(ComponentType::DECAL));
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
	if (mFleeToAttackTimer.Delay(mFleeToAttackTime))
	{
		mIsFleeing = false;
		mCurrentState = EnemyState::IDLE;
		return;
	}

	mAnimationComponent->SetAnimSpeed(2.5f);
	if (mAiAgentComponent && !mIsFleeing)
	{
		mAiAgentComponent->FleeFromTarget(mPlayer->GetWorldPosition());
		mGameObject->LookAt(mGameObject->GetWorldPosition() + mAiAgentComponent->GetDirection());
	}

	//if (!IsPlayerInRange(mFleeRadius))
	//{
	//	mIsFleeing = false;
	//	mCurrentState = EnemyState::IDLE;
	//}
}

void Rat::Death()
{
	if (mDeathTimer.DelayWithoutReset(mDeathTime))
	{
		mGameObject->SetEnabled(false);
		return;
	}

	float halfDeathTime = mDeathTime / 2.0f;
	if (mDeathTimer.GetTimePassed() >= halfDeathTime)
	mDecalComponent->SetFadeFactor(1.0f - ((mDeathTimer.GetTimePassed() - halfDeathTime) / halfDeathTime));
}


void Rat::OnCollisionEnter(CollisionData* collisionData)
{
	if (collisionData->collidedWith->GetTag() == "Door" || collisionData->collidedWith->GetTag() == "Bridge")
	{
		mAiAgentComponent->FleeFromTarget(collisionData->collidedWith->GetWorldPosition());
		mGameObject->LookAt(mGameObject->GetWorldPosition() + mAiAgentComponent->GetDirection());
		mIsFleeing = true;
	}

	if (collisionData->collidedWith->GetTag() == "Player")
	{
		if (mGameObject->GetWorldPosition().Distance(collisionData->collidedWith->GetWorldPosition()) < 0.4f)
		{
			mCurrentState = EnemyState::DEATH;
			for (int i = 0; i < mMeshComponents.size(); ++i)
			{
				mMeshComponents[i]->SetEnable(false);
			}
			if (mDecalComponent)
			{
				mDecalComponent->SetEnable(true);
				mDecalComponent->SetFadeFactor(1.0f);
			}
			if (mDeathParticles) mDeathParticles->SetEnable(true);
			if (mAiAgentComponent) mAiAgentComponent->PauseCrowdNavigation();
		}
	}
}
