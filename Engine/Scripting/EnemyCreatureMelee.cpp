#include "EnemyCreatureMelee.h"
#include "Application.h"
#include "ModuleDetourNavigation.h"

#include "GameObject.h"
#include "ScriptComponent.h"
#include "AnimationComponent.h"
#include "AIAGentComponent.h"
#include "BoxColliderComponent.h"

#include "GameManager.h"
#include "AudioManager.h"
#include "PlayerController.h"

CREATE(EnemyCreatureMelee)
{
	CLASS(owner);
	SEPARATOR("STATS");
	MEMBER(MemberType::FLOAT, mMaxHealth);
	MEMBER(MemberType::FLOAT, mSpeed);
	MEMBER(MemberType::FLOAT, mRotationSpeed);
	MEMBER(MemberType::FLOAT, mAttackDamage);
	MEMBER(MemberType::FLOAT, mAttackDuration);
	MEMBER(MemberType::FLOAT, mChargeDuration);
	MEMBER(MemberType::FLOAT, mAttackCoolDown);
	SEPARATOR("STATES");
	MEMBER(MemberType::FLOAT, mAttackDistance);

	END_CREATE;
}

void EnemyCreatureMelee::Start()
{
	Enemy::Start();
	Init();

	mCollider = static_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
	if (mCollider)
	{
		mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&EnemyCreatureMelee::OnCollisionEnter, this, std::placeholders::_1)));
	}

	mDisengageTime = 0.0f;
	mDeathTime = 1.0f;
}

void EnemyCreatureMelee::Chase()
{
	PlayStepAudio();
	if (IsPlayerInRange(mChaseDistance))
	{
		if (mAiAgentComponent)
		{
			if (IsPlayerInRange(mAttackDistance))
			{	
				//TODO: CHANGE WITH GOOD ROTATION BEHAVIOUR
				mAiAgentComponent->SetNavigationPath(mGameObject->GetWorldPosition());
				float3 direction = (mPlayer->GetWorldPosition() - mGameObject->GetWorldPosition());
				direction.y = 0;
				direction.Normalize();
				float angle = std::atan2(direction.x, direction.z);;

				if (mGameObject->GetWorldRotation().y != angle)
				{
					mGameObject->SetWorldRotation(float3(0, angle, 0));
				}
				if (mAttackCoolDownTimer.Delay(mAttackCoolDown))
				{
					mCurrentState = EnemyState::CHARGE;
				}
			}
			else
			{
				//TODO: CHANGE WITH GOOD ROTATION BEHAVIOUR
				mAiAgentComponent->SetNavigationPath(mPlayer->GetWorldPosition());
				float3 direction = (mPlayer->GetWorldPosition() - mGameObject->GetWorldPosition());
				direction.y = 0;
				direction.Normalize();
				float angle = std::atan2(direction.x, direction.z);;

				if (mGameObject->GetWorldRotation().y != angle)
				{
					mGameObject->SetWorldRotation(float3(0, angle, 0));
				}
			}	
		}
	}
	else
	{
		mCurrentState = EnemyState::IDLE;
	}
}

void EnemyCreatureMelee::Charge()
{
	if (mAiAgentComponent) mAiAgentComponent->PauseCrowdNavigation();
	Enemy::Charge();
	mHit = false;

}

void EnemyCreatureMelee::Attack()
{
	if (mAttackDurationTimer.Delay(mAttackDuration))
	{
		if (mAiAgentComponent) mAiAgentComponent->StartCrowdNavigation();
		mCurrentState = EnemyState::CHASE;
	}
	
	float movement = (mAttackDistance * App->GetDt()) / mAttackDuration;

	mGameObject->SetWorldPosition(App->GetNavigation()->FindNearestPoint(mGameObject->GetWorldPosition() + mGameObject->GetFront() * movement, float3(10.0f)));
}


void EnemyCreatureMelee::OnCollisionEnter(CollisionData* collisionData)
{
	if (mCurrentState == EnemyState::ATTACK && !mHit && collisionData->collidedWith->GetTag() == "Player")
	{
		mHit = true;
		GameManager::GetInstance()->GetPlayerController()->TakeDamage(mAttackDamage);
	}
}