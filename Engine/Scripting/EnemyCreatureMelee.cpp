#include "EnemyCreatureMelee.h"
#include "Application.h"
#include "AIAGentComponent.h"
#include "AnimationComponent.h"
#include "BoxColliderComponent.h"
#include "GameObject.h"

#include "GameManager.h"
#include <MathFunc.h>


CREATE(EnemyCreatureMelee)
{
	CLASS(owner);
	SEPARATOR("STATS");
	MEMBER(MemberType::FLOAT, mMaxHealth);
	MEMBER(MemberType::FLOAT, mSpeed);
	MEMBER(MemberType::FLOAT, mRotationSpeed);
	MEMBER(MemberType::INT, mShieldDropRate);
	MEMBER(MemberType::FLOAT, mChaseDelay);
	MEMBER(MemberType::FLOAT, mMeleeAttackCoolDown);
	MEMBER(MemberType::FLOAT, mMeleeDistance);
	MEMBER(MemberType::FLOAT, mMeeleDamage);

	END_CREATE;
}



EnemyCreatureMelee::EnemyCreatureMelee(GameObject* owner) : Enemy(owner)
{
}

void EnemyCreatureMelee::Start()
{
	Enemy::Start();

	mAiAgentComponent = reinterpret_cast<AIAgentComponent*>(mGameObject->GetComponent(ComponentType::AIAGENT));

	mAnimationComponent = reinterpret_cast<AnimationComponent*>(mGameObject->GetComponent(ComponentType::ANIMATION));
	if (mAnimationComponent)
	{
		mAnimationComponent->SetIsPlaying(true);

	}
	mCollider = reinterpret_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));

	if (mCollider)
	{
		mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&EnemyCreatureMelee::OnCollisionEnter, this, std::placeholders::_1)));
	}
}

void EnemyCreatureMelee::Update()
{
	if (GameManager::GetInstance()->IsPaused()) return;

	Enemy::Update();

	if (!mBeAttracted)
	{
		switch (mCurrentState)
		{
		case EnemyState::IDLE:
			mAnimationComponent->SendTrigger("tIdle", 0.2f);
			Idle();

			break;
		case EnemyState::CHASE:
			mAnimationComponent->SendTrigger("tChase", 0.2f);
			Chase();
			break;
		case EnemyState::ATTACK:
			mAnimationComponent->SendTrigger("tAttack", 0.2f);
			Attack();
			break;
		}
	}

	mBeAttracted = false;
}

void EnemyCreatureMelee::Idle()
{
	if (IsPlayerInRange(mActivationRange))
	{
		mCurrentState = EnemyState::CHASE;
		//mAiAgentComponent->SetNavigationPath(mPlayer->GetWorldPosition());
	}
}

void EnemyCreatureMelee::Chase()
{
	PlayStepAudio();
	if (IsPlayerInRange(mActivationRange))
	{
		if (mAiAgentComponent)
		{
			float rotationSpeed = 100.0f * App->GetDt();

			mAiAgentComponent->SetNavigationPath(mPlayer->GetWorldPosition());

			float3 oldPosition = mGameObject->GetWorldPosition();
			mAiAgentComponent->MoveAgent(mSpeed);

			float3 direction = (mGameObject->GetWorldPosition() - oldPosition);
			direction.y = 0;
			direction.Normalize();
			float angle = std::atan2(direction.x, direction.z);
			float eulerAngle = RadToDeg(angle);

			float currentAngle = mGameObject->GetWorldEulerAngles().y;
			float currentAngleEuler = RadToDeg(currentAngle);
			//mGameObject->SetLocalRotation(float3(0, DegToRad(currentAngleEuler + 10.0f * App->GetDt()), 0));
			
			if (eulerAngle > currentAngleEuler)
			{
				if (currentAngleEuler + rotationSpeed <= eulerAngle)
				{
					mGameObject->SetWorldRotation(float3(0, DegToRad(currentAngleEuler + rotationSpeed), 0));
				}
				else
				{
					mGameObject->SetWorldRotation(float3(0, angle, 0));
				}

			}
			else if (eulerAngle <= currentAngleEuler)
			{
				if (currentAngleEuler - rotationSpeed >= eulerAngle)
				{
					mGameObject->SetWorldRotation(float3(0, DegToRad(currentAngleEuler - rotationSpeed), 0));
				}
				else
				{
					mGameObject->SetWorldRotation(float3(0, angle, 0));
				}

			}


		}
		if (IsPlayerInRange(mMeleeDistance))
		{
			// mCurrentState = EnemyState::ATTACK;
		}
	}
	else
	{
		mCurrentState = EnemyState::IDLE;
	}
}

void EnemyCreatureMelee::Attack()
{
}

bool EnemyCreatureMelee::IsMoving()
{
	return (mCurrentState == EnemyState::CHASE);
}

void EnemyCreatureMelee::MeleeAttack()
{
}

void EnemyCreatureMelee::Death()
{
}

void EnemyCreatureMelee::Init()
{
}

void EnemyCreatureMelee::OnCollisionEnter(CollisionData* collisionData)
{
}

void EnemyCreatureMelee::PlayStepAudio()
{
}

void EnemyCreatureMelee::PlayMeleeAudio()
{
}
