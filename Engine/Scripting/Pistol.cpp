#include "Pistol.h"
#include "Application.h"

#include "ModuleScene.h"
#include "ModuleInput.h"

#include "GameObject.h"
#include "ScriptComponent.h"
#include "Trail.h"

#include "Physics.h"
#include "Geometry/Ray.h"

#include "GameManager.h"
#include "AudioManager.h"
#include "PoolManager.h"
#include "PlayerController.h"
#include "HudController.h"
#include "Enemy.h"
#include "RayCastBullet.h"


Pistol::Pistol() : RangeWeapon()
{
	mCurrentAmmo = 16;
	mMaxAmmo = 16;
	mDamage = 2.0f;
	mAttackDuration = 0.0f;
	mAttackCooldown = 0.15f;
	mAttackRange = 25.0f;

	mBulletSpeed = 50.0f;
	mBulletMaxSpread = 0.2f;

	mFire = App->GetScene()->InstantiatePrefab("PistolFire.prfb", GameManager::GetInstance()->GetPlayerController()->GetShootOriginGO());
	if (mFire)	mFire->SetEnabled(false);

	mCameraShakeDuration = 0.15f;
	mCameraShakeStrengh = 0.03f;

}

void Pistol::Enter()
{
	//CONTROLLER VIBRATION
	App->GetInput()->SetGameControllerRumble(45000, 0, 100);
}

void Pistol::Exit()
{
	if (mFire)
	{
		mFire->SetEnabled(false);
	}
}

void Pistol::Attack(float time)
{
	//Audio
	if (GameManager::GetInstance()->GetAudio())
	{
		PlayHitSound();
	}

	mCurrentAmmo--;
	GameManager::GetInstance()->GetHud()->SetAmmo(mCurrentAmmo);
	
	Shoot(GameManager::GetInstance()->GetPlayerController()->GetShootOriginGO()->GetWorldPosition(), mBulletMaxSpread, PoolType::BULLET);
	
	//Fire Particles
	if (mFire)
	{
		mFire->SetEnabled(false);
		mFire->SetEnabled(true);
	}
}

void Pistol::Reload()
{
	mCurrentAmmo = mMaxAmmo;
	GameManager::GetInstance()->GetHud()->SetAmmo(mCurrentAmmo);
}

void Pistol::PlayHitSound()
{
	GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::PLAYER_PISTOL, GameManager::GetInstance()->GetPlayer()->GetWorldPosition());
}
