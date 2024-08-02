#include "Pistol.h"
#include "Application.h"

#include "ModuleScene.h"
#include "ModuleInput.h"

#include "GameObject.h"
#include "ScriptComponent.h"
#include "TrailComponent.h"

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
	mBulletMaxSpread = 0.3f;

	mFire = App->GetScene()->InstantiatePrefab("PistolFire.prfb");
	if (mFire)	mFire->SetEnabled(false);
}

void Pistol::Enter()
{
	//CONTROLLER VIBRATION
	App->GetInput()->SetGameControllerRumble(45000, 0, 100);
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
	
	ColorGradient gradient;
	gradient.AddColorGradientMark(0.1f, float4(0.0f, 1.0f, 0.0f, 1.0f));
	Shoot(GameManager::GetInstance()->GetPlayerController()->GetShootOriginGO()->GetWorldPosition(), mBulletMaxSpread, gradient);
	
	//Fire Particles
	if (mFire)
	{
		mFire->SetEnabled(false);
		mFire->SetEnabled(true);
		mFire->SetWorldPosition(GameManager::GetInstance()->GetPlayerController()->GetShootOriginGO()->GetWorldPosition());
	}
}

void Pistol::Reload()
{
	mCurrentAmmo = mMaxAmmo;
	GameManager::GetInstance()->GetHud()->SetAmmo(mCurrentAmmo);
}

void Pistol::PlayHitSound()
{
	GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::GUNFIRE, GameManager::GetInstance()->GetPlayer()->GetWorldPosition());
}
