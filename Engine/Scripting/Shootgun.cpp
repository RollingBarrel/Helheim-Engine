#include "Shootgun.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleInput.h"

#include "GameObject.h"
#include "ScriptComponent.h"
#include "Trail.h"
#include "Physics.h"

#include "Geometry/Ray.h"
#include "Algorithm/Random/LCG.h"

#include "GameManager.h"
#include "PlayerController.h"
#include "HudController.h"
#include "PoolManager.h"
#include "AudioManager.h"
#include "RayCastBullet.h"


Shootgun::Shootgun()
{
    mDamage = 1.0f;
    mAttackRange = 25.0f;
    mAttackDuration = 0.0f;
    mAttackCooldown = 0.5f;

    mBulletSpeed = 30.0f;
    mBulletMaxSpread = 1.0f;
    mEnergyCost = 10;
    mFire = App->GetScene()->InstantiatePrefab("ShootgunFire.prfb", GameManager::GetInstance()->GetPlayerController()->GetShootOriginGO());
    if (mFire)
    {
        mFire->SetEnabled(false);
    }

    mCameraShakeDuration = 0.2f;
    mCameraShakeStrengh = 0.1f;
}

void Shootgun::Enter()
{
    //CONTROLLER VIBRATION
    App->GetInput()->SetGameControllerRumble(50000, 0, 150);
}

void Shootgun::Exit()
{
    if (mFire)
    {
        mFire->SetEnabled(false);
    }
}

void Shootgun::Attack(float time)
{
    //LOG("ShotGun Attack");
   
    //Audio
    if (GameManager::GetInstance()->GetAudio())
    {
        PlayHitSound();
    }

    //Shoot Logic
    int numBullets = 10;
    GameManager::GetInstance()->GetPlayerController()->UseEnergy(mEnergyCost);
    for (int i = 0; i < numBullets; ++i)
    {
        Shoot(GameManager::GetInstance()->GetPlayerController()->GetShootOriginGO()->GetWorldPosition(), mBulletMaxSpread, PoolType::RED_BULLET);
    }
    
    //Fire Particles
    if (mFire)
    {
        mFire->SetEnabled(false);
        mFire->SetEnabled(true);
    }
}

void Shootgun::PlayHitSound()
{
    GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::PLAYER_SHOTGUN, GameManager::GetInstance()->GetPlayer()->GetWorldPosition());
}
