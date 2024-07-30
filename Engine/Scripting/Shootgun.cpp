#include "Shootgun.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleInput.h"

#include "GameObject.h"
#include "ScriptComponent.h"
#include "TrailComponent.h"
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
    mDamage = 2.0f;
    mAttackRange = 25.0f;
    mAttackDuration = 0.0f;
    mAttackCooldown = 0.5f;

    mBulletSpeed = 30.0f;

    mFire = App->GetScene()->InstantiatePrefab("ShootgunFire.prfb");
    if (mFire)
    {
        mFire->SetEnabled(false);
    }

}

Shootgun::~Shootgun()
{
}

void Shootgun::Enter()
{
    //CONTROLLER VIBRATION
    App->GetInput()->SetGameControllerRumble(50000, 0, 150);
}

void Shootgun::Attack(float time)
{
    LOG("ShotGun Attack");
    int numBullets = 10;

    //TODO: Rethink this
    reinterpret_cast<PlayerController*>(reinterpret_cast<ScriptComponent*>(GameManager::GetInstance()->GetPlayer()->GetComponent(ComponentType::SCRIPT))->GetScriptInstance())->UseEnergy(numBullets);
   

    //Audio
    if (GameManager::GetInstance()->GetAudio())
    {
        PlayHitSound();
    }

    ColorGradient gradient;
    gradient.AddColorGradientMark(0.1f, float4(1.0f, 0.62f, 0.275f, 1.0f));
    gradient.AddColorGradientMark(0.6f, float4(1.0f, 0.0f, 0.0f, 1.0f));

    //Shoot Logic
    for (int i = 0; i < numBullets; ++i)
    {
        float3 spread = float3::zero;
        spread += GameManager::GetInstance()->GetPlayer()->GetUp() * LCG().Float(-1.0f, 1.0f);
        spread += GameManager::GetInstance()->GetPlayer()->GetRight() * LCG().Float(-1.0f, 1.0f);

        float3 rayDirection = GameManager::GetInstance()->GetPlayer()->GetFront() + spread.Normalized() * LCG().Float(0.0f, 0.2f);;

        Shoot(GameManager::GetInstance()->GetPlayerController()->GetShootOriginGO()->GetWorldPosition(), rayDirection , gradient);
    }
    
    //Fire Particles
    if (mFire)
    {
        mFire->SetEnabled(false);
        mFire->SetEnabled(true);
        mFire->SetWorldPosition(GameManager::GetInstance()->GetPlayerController()->GetShootOriginGO()->GetWorldPosition());
    }
}

void Shootgun::Exit()
{
}

void Shootgun::Reload()
{
    //mCurrentAmmo = mMaxAmmo;
    //GameManager::GetInstance()->GetHud()->SetAmmo(mCurrentAmmo);
}

void Shootgun::PlayHitSound()
{
    GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::GUNFIRE, GameManager::GetInstance()->GetPlayer()->GetWorldPosition());
}
