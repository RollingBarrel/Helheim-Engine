#include "Machinegun.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleInput.h"

#include "GameObject.h"
#include "ScriptComponent.h"
#include "TrailComponent.h"
#include "Physics.h"

#include "Geometry/Ray.h"

#include "GameManager.h"
#include "PlayerController.h"
#include "HudController.h"
#include "PoolManager.h"
#include "AudioManager.h"
#include "RayCastBullet.h"


Machinegun::Machinegun()
{
	mDamage = 4.0f;
	mAttackDuration = 0.3f;
    mAttackCooldown = 0.35f;
	mAttackRange = 25.0f;
    mNumBullets = 3;

    mBulletSpeed = 60.0f;
    mBulletMaxSpread = 0.4f;

    mShootDuration = mAttackDuration / static_cast<float>(mNumBullets);

    mFire = App->GetScene()->InstantiatePrefab("MachingunFire.prfb");
    if (mFire)
    {
        mFire->SetEnabled(false);
    }
}

void Machinegun::Enter()
{
    //CONTROLLER VIBRATION
    App->GetInput()->SetGameControllerRumble(40000, 0, 100);
}

void Machinegun::Attack(float time)
{
    float delay = mShootDuration;
    if (mFirstShoot)
    {
        mFirstShoot = false;
        delay = 0.0f;
    }
   
    if (mShootTimer.Delay(delay))
    {
        GameManager::GetInstance()->GetPlayerController()->UseEnergy(mNumBullets);
        
        //Audio
        if (GameManager::GetInstance()->GetAudio())
        {
            PlayHitSound();
        }

        ColorGradient gradient;
        gradient.AddColorGradientMark(0.1f, float4(0.686f, 0.0f, 1.0f, 1.0f));
        gradient.AddColorGradientMark(0.6f, float4(0.0f, 0.0f, 1.0f, 1.0f));
        Shoot(GameManager::GetInstance()->GetPlayerController()->GetShootOriginGO()->GetWorldPosition(), mBulletMaxSpread, gradient);


        //Fire Particles
        if (mFire)
        {
            mFire->SetEnabled(false);
            mFire->SetEnabled(true);
            mFire->SetWorldPosition(GameManager::GetInstance()->GetPlayerController()->GetShootOriginGO()->GetWorldPosition());
        }
    }
}

void Machinegun::Exit()
{
    mFirstShoot = true;
}

void Machinegun::PlayHitSound()
{
    GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::GUNFIRE, GameManager::GetInstance()->GetPlayer()->GetWorldPosition());
}