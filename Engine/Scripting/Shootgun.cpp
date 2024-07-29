#include "Shootgun.h"
#include "GameManager.h"
#include "PoolManager.h"
#include "HudController.h"
#include "AudioManager.h"
#include "Enemy.h"

#include "GameObject.h"
#include "ScriptComponent.h"
#include "TrailComponent.h"
#include "PlayerController.h"
#include "RayCastBullet.h"

#include "Application.h"
#include "ModuleScene.h"

#include "Physics.h"
#include "Geometry/Ray.h"
#include "Algorithm/Random/LCG.h"

#include "ModuleInput.h"

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

    //Shoot Logic

    for (int i = 0; i < numBullets; ++i)
    {
        Ray ray;
        ray.pos = GameManager::GetInstance()->GetPlayer()->GetWorldPosition();
        ray.pos.y++;
        ray.dir = GameManager::GetInstance()->GetPlayer()->GetFront();

        float3 spread = float3::zero;
        spread += GameManager::GetInstance()->GetPlayer()->GetUp() * LCG().Float(-1.0f, 1.0f);
        spread += GameManager::GetInstance()->GetPlayer()->GetRight() * LCG().Float(-1.0f, 1.0f);

        ray.dir += spread.Normalized() * LCG().Float(0.0f, 0.2f);

        Hit hit;
        std::vector<std::string> ignoreTags = { "Bullet", "BattleArea", "Trap", "Drop" };
        Physics::Raycast(hit, ray, mAttackRange, &ignoreTags);


        //PARTICLES
        if (GameManager::GetInstance()->GetPoolManager())
        {
            GameObject* bullet = GameManager::GetInstance()->GetPoolManager()->Spawn(PoolType::BULLET);
            RayCastBullet* bulletScript = reinterpret_cast<RayCastBullet*>(reinterpret_cast<ScriptComponent*>(bullet->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
            
            ColorGradient gradient;
            gradient.AddColorGradientMark(0.1f, float4(1.0f, 0.62f, 0.275f, 1.0f));
            gradient.AddColorGradientMark(0.6f, float4(1.0f, 0.0f, 0.0f, 1.0f));

            bullet->SetEnabled(false);
            if (hit.IsValid())
            {
                bulletScript->Init(ray.pos, hit, mDamage, mBulletSpeed,  mBulletSize, &gradient);
            }
            else
            {
                bulletScript->Init(ray.pos, ray.pos + ray.dir.Mul(mAttackRange), mDamage, mBulletSpeed, mBulletSize, &gradient);
            }
        }
    }
    
    //PARTICLES
    if (mFire)
    {
        mFire->SetEnabled(false);
        mFire->SetEnabled(true);
        mFire->SetWorldPosition(GameManager::GetInstance()->GetPlayer()->GetWorldPosition() + float3(0.0f,1.0f,0.0f) + GameManager::GetInstance()->GetPlayer()->GetFront());
    }


    LOG("Missed bullets = %i", count);
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
