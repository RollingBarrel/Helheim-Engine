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
	SEPARATOR("VFX");
	MEMBER(MemberType::GAMEOBJECT, mUltHitEffectGO);
	MEMBER(MemberType::GAMEOBJECT, mDashAttackVFX);
	MEMBER(MemberType::GAMEOBJECT, mDashRightVFX);
	MEMBER(MemberType::GAMEOBJECT, mDashLeftVFX);
	MEMBER(MemberType::GAMEOBJECT, mDashIndicator);
	MEMBER(MemberType::GAMEOBJECT, mDashIndicatorFinal);
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

	mAttackAudioPlayed = false;
	mDeathAudioPlayed = false;
	mDisengageTime = 0.0f;
	mDeathTime = 2.20f;
}

void EnemyCreatureMelee::Update()
{
	Enemy::Update();
	if (mCurrentState == EnemyState::ATTACK && !mAttackAudioPlayed)
	{
		mAttackAudioPlayed = true;
		GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ENEMY_CREATURE_CHARGE_ATTACK, mGameObject->GetWorldPosition());
	}

	if (mAttackCoolDownTimer.DelayWithoutReset(mAttackCoolDown))
	{
		mAttack = true;
	}

}

void EnemyCreatureMelee::Chase()
{
	if (mAiAgentComponent)
	{
		PlayStepAudio();
		if (IsPlayerReachable())
		{
			mAiAgentComponent->SetNavigationPath(mGameObject->GetWorldPosition());
			RotateHorizontally(mPlayer->GetWorldPosition(), mRotationSpeed);
			if (mAttack)
			{
				RotateHorizontally(mPlayer->GetWorldPosition(), 100.0f);
				mCurrentState = EnemyState::CHARGE;
				mDashAttackVFX->SetEnabled(true);
				mDashRightVFX->SetEnabled(true);
				mDashLeftVFX->SetEnabled(true);
				mDashIndicator->SetEnabled(true);
				mDashIndicatorFinal->SetLocalPosition(float3(0, 0, 0));
			}
		}
		else
		{
			mAiAgentComponent->SetNavigationPath(mPlayer->GetWorldPosition());
			float3 dir = mAiAgentComponent->GetDirection();
			if (!dir.Equals(float3::zero)) mGameObject->LookAt(mGameObject->GetWorldPosition() + float3(dir.x, 0.0f, dir.z));
		}
	}
}

void EnemyCreatureMelee::Charge()
{
	if (mAiAgentComponent) mAiAgentComponent->PauseCrowdNavigation();
	mDashIndicatorFinal->SetLocalPosition(float3(0,0,mAttackDistance * mChargeDurationTimer.GetTimePassed() / mChargeDuration));
	Enemy::Charge();
	mHit = false;
}

void EnemyCreatureMelee::Attack()
{
	if (mAttackDurationTimer.Delay(mAttackDuration))
	{
		if (mAiAgentComponent) mAiAgentComponent->StartCrowdNavigation();
		mAttackCoolDownTimer.Reset();
		mAttack = false;
		mCurrentState = EnemyState::CHASE;
		mDashAttackVFX->SetEnabled(false);
		mDashRightVFX->SetEnabled(false);
		mDashLeftVFX->SetEnabled(false);
		mDashIndicator->SetEnabled(false);
		mAttackAudioPlayed = false;
	}

	float movement = (mAttackDistance * App->GetDt()) / mAttackDuration;
	mGameObject->SetWorldPosition(App->GetNavigation()->FindNearestPoint(mGameObject->GetWorldPosition() + mGameObject->GetFront() * movement, float3(10.0f)));
	mDashIndicatorFinal->SetLocalPosition(float3(0, 0, mAttackDistance * (1 - mAttackDurationTimer.GetTimePassed() / mAttackDuration)));
}

void EnemyCreatureMelee::TakeDamage(float damage)
{
	Enemy::TakeDamage(damage);
	GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ENEMY_CREATURE_HIT, mGameObject->GetWorldPosition());
}

void EnemyCreatureMelee::Death()
{
	Enemy::Death();
	mDashLeftVFX->SetEnabled(false);
	mDashRightVFX->SetEnabled(false);
	mDashIndicator->SetEnabled(false);
	if (!mDeathAudioPlayed)
	{
		GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ENEMY_CREATURE_DEATH, mGameObject->GetWorldPosition());
		mDeathAudioPlayed = true;
	}
}

void EnemyCreatureMelee::SetAttracted(bool attracted)
{
	Enemy::SetAttracted(attracted);

	if (mCurrentState == EnemyState::CHARGE || mCurrentState == EnemyState::ATTACK)
		mDashIndicator->SetEnabled(!attracted);
	
}

void EnemyCreatureMelee::OnCollisionEnter(CollisionData* collisionData)
{
	if (mCurrentState == EnemyState::ATTACK && !mHit && collisionData->collidedWith->GetTag() == "Player")
	{
		mHit = true;
		GameManager::GetInstance()->GetPlayerController()->TakeDamage(mAttackDamage);
	}
}