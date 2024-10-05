#include "EnemyCreatureRange.h"
#include "Application.h"
#include "GameObject.h"
#include "AnimationComponent.h"
#include "ScriptComponent.h"
#include "AIAGentComponent.h"
#include "LineComponent.h"

#include "GameManager.h"
#include "AudioManager.h"
#include "PoolManager.h"
#include "PlayerController.h"

#include "ColorGradient.h"
#include "Physics.h"
#include "Geometry/Ray.h"
#include <MathFunc.h>


CREATE(EnemyCreatureRange)
{
	CLASS(owner);
	SEPARATOR("STATS");
	MEMBER(MemberType::FLOAT, mMaxHealth);
	MEMBER(MemberType::FLOAT, mSpeed);
	MEMBER(MemberType::FLOAT, mRotationSpeed);
	MEMBER(MemberType::FLOAT, mChargeDuration);
	MEMBER(MemberType::FLOAT, mAttackDamage);
	MEMBER(MemberType::FLOAT, mAttackDuration);
	MEMBER(MemberType::FLOAT, mAttackRotationSpeed);
	MEMBER(MemberType::FLOAT, mAttackCoolDown);
	SEPARATOR("STATES");
	MEMBER(MemberType::FLOAT, mAttackDistance);
	SEPARATOR("GAME OBJECTS");
	MEMBER(MemberType::GAMEOBJECT, mLaserOrigin);
	MEMBER(MemberType::GAMEOBJECT, mLaserEnd);
	MEMBER(MemberType::GAMEOBJECT, mLaserCharge);
	MEMBER(MemberType::GAMEOBJECT, mPreviewOrigin);
	MEMBER(MemberType::GAMEOBJECT, mPreviewEnd);
	MEMBER(MemberType::GAMEOBJECT, mPreviewContrast);
	MEMBER(MemberType::GAMEOBJECT, mPreviewContrastEnd);
	SEPARATOR("VFX");
	MEMBER(MemberType::GAMEOBJECT, mUltHitEffectGO);
	END_CREATE;
}

void EnemyCreatureRange::Start()
{
	Enemy::Start();
	Init();

	if (mLaserOrigin)	mLaserOrigin->SetEnabled(false);
	if (mLaserEnd) mLaserEnd->SetEnabled(false);
	if (mLaserCharge)
	{
		mLaserCharge->SetEnabled(false);
		if (mLaserOrigin) mLaserCharge->SetLocalPosition(mLaserOrigin->GetLocalPosition());
	}
	if (mPreviewOrigin)
	{
		mPreviewOrigin->SetEnabled(false);
		mPreviewLine = reinterpret_cast<LineComponent*>(mPreviewOrigin->GetComponent(ComponentType::LINE));
	}
	if (mPreviewEnd)	mPreviewEnd->SetEnabled(false);

	if (mPreviewContrast)	    mPreviewContrast->SetEnabled(false);
	if (mPreviewContrastEnd)	mPreviewContrastEnd->SetEnabled(false);

	mDeathAudioPlayed = false;
	mDeathTime = 2.20f;
	mAimTime = mChargeDuration * 0.8f;
	mLaserSound = GameManager::GetInstance()->GetAudio()->Play(SFX::ENEMY_CREATURE_LASER, mLaserSound, mGameObject->GetWorldPosition());
	GameManager::GetInstance()->GetAudio()->Pause(SFX::ENEMY_CREATURE_LASER, mLaserSound,true);
}

void EnemyCreatureRange::Update()
{
	Enemy::Update();

	if (mCurrentState != EnemyState::ATTACK)
	{
		GameManager::GetInstance()->GetAudio()->Pause(SFX::ENEMY_CREATURE_LASER, mLaserSound, true);

		if (mLaserOrigin)	mLaserOrigin->SetEnabled(false);
		if (mLaserEnd) mLaserEnd->SetEnabled(false);
	}
	if (mCurrentState != EnemyState::CHARGE)
	{
		if (mLaserCharge)			mLaserCharge->SetEnabled(false);
		if (mPreviewOrigin)		    mPreviewOrigin->SetEnabled(false);
		if (mPreviewEnd)			mPreviewEnd->SetEnabled(false);
		if (mPreviewContrast)	    mPreviewContrast->SetEnabled(false);
		if (mPreviewContrastEnd)	mPreviewContrastEnd->SetEnabled(false);
		mPreviewWidth = 0.0f;
	}

	if (mAttackCoolDownTimer.DelayWithoutReset(mAttackCoolDown))
	{
		mDoDamage = true;
	}
}

void EnemyCreatureRange::Charge()
{
	Enemy::Charge();

	if (!mAimTimer.DelayWithoutReset(mAimTime))
	{
		mGameObject->LookAt(mPlayer->GetWorldPosition());
	}
	
	if (mLaserCharge)			mLaserCharge->SetEnabled(true); 
	if (mPreviewOrigin)			mPreviewOrigin->SetEnabled(true);
	if (mPreviewEnd)			mPreviewEnd->SetEnabled(true);
	if (mPreviewContrast)	    mPreviewContrast->SetEnabled(true);
	if (mPreviewContrastEnd)	mPreviewContrastEnd->SetEnabled(true);

	SetPreviewWidth();
	LaserCollide(mPreviewContrast, mPreviewContrastEnd, false);
	LaserCollide(mPreviewOrigin, mPreviewEnd, false);
}

void EnemyCreatureRange::Attack()
{
	Enemy::Attack();

	GameManager::GetInstance()->GetAudio()->Pause(SFX::ENEMY_CREATURE_LASER, mLaserSound, false);
	GameManager::GetInstance()->GetAudio()->SetPosition(SFX::ENEMY_CREATURE_LASER, mLaserSound, mGameObject->GetWorldPosition());
	
	RotateHorizontally(mPlayer->GetWorldPosition(), mAttackRotationSpeed);
	mAimTimer.Reset();

	mAnimationComponent->SendTrigger("tAttack", 0.2f);
	
	if (mLaserOrigin)	mLaserOrigin->SetEnabled(true);
	if (mLaserEnd)		mLaserEnd->SetEnabled(true);

	LaserCollide(mLaserOrigin, mLaserEnd, true);
}

void EnemyCreatureRange::Death()
{
	Enemy::Death(); 
	if (!mDeathAudioPlayed)
	{
		GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ENEMY_CREATURE_DEATH, mGameObject->GetWorldPosition());
		mDeathAudioPlayed = true;
	}
}

void EnemyCreatureRange::TakeDamage(float damage)
{
	Enemy::TakeDamage(damage);
	GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ENEMY_CREATURE_HIT, mGameObject->GetWorldPosition());
}

void EnemyCreatureRange::SetPreviewWidth()
{
	//TODO:
	/*if (mPreviewLine)
	{ 
		mPreviewWidth = Lerp(mPreviewWidth, 100.0f, App->GetDt());
		mPreviewLine->SetLineWidth(10.0f);
	}*/
}

void EnemyCreatureRange::LaserCollide(GameObject* origin, GameObject* end, bool dealDamage)
{
	Hit hit;
	Ray ray;
	ray.dir = mGameObject->GetFront();
	ray.pos = origin->GetWorldPosition();

	std::vector<std::string> ignoreTags = { "Bullet", "BattleArea", "Trap", "Drop", "Bridge", "DoorArea", "Collectible" };
	Physics::Raycast(hit, ray, mAttackDistance, &ignoreTags);
	if (hit.IsValid())
	{
		if (hit.mGameObject->GetTag().compare("Player") == 0)
		{
			PlayerController* player = GameManager::GetInstance()->GetPlayerController();
			if (!player->IsPlayerDashing())
			{
				end->SetWorldPosition(hit.mHitPoint);
				if (mDoDamage)
				{
					mAttackCoolDownTimer.Reset();
					if (dealDamage) player->TakeDamage(mAttackDamage);
					mDoDamage = false;
				}
			}
		}
		else
		{
			end->SetWorldPosition(hit.mHitPoint);
		}

	}
	else
	{
		float3 originPosition = origin->GetLocalPosition();
		end->SetLocalPosition(float3(originPosition.x, originPosition.y, originPosition.z + mAttackDistance));
	}
}


