#include "Machinegun.h"
#include "Application.h"
#include "GameManager.h"
#include "HudController.h"
#include "PoolManager.h"
#include "AudioManager.h"
#include "Bullet.h"
#include "Enemy.h"

#include "GameObject.h"
#include "ScriptComponent.h"
#include "TrailComponent.h"
#include "Physics.h"

#include "Application.h"
#include "ModuleScene.h"

#include "Geometry/Ray.h"
#include "Algorithm/Random/LCG.h"
#include <PlayerController.h>

Machinegun::Machinegun()
{
	mAttackRange = 100.0f;
	mDamage = 0.4f;
	mAttackDuration = 0.7f;
    mAttackCooldown = 0.3f;
    mNumBullets = 3;

    mShootDuration = mAttackDuration / static_cast<float>(mNumBullets);

    mFire = App->GetScene()->InstantiatePrefab("MachingunFire.prfb");
    if (mFire)
    {
        mFire->SetEnabled(false);
    }
}

Machinegun::~Machinegun()
{
}

void Machinegun::Enter()
{
}

void Machinegun::Attack(float time)
{
    LOG("MachineGun Attack");

    float delay = mShootDuration;
    if (mFirstShoot)
    {
        mFirstShoot = false;
        delay = 0.0f;
    }
   
    if (Delay(delay))
    {
        //Audio
        if (GameManager::GetInstance()->GetAudio())
        {
            PlayHitSound();
        }

        //Shoot Logic
        Ray ray;
        ray.pos = GameManager::GetInstance()->GetPlayer()->GetPosition();
        ray.pos.y++;
        ray.dir = GameManager::GetInstance()->GetPlayer()->GetFront();

        //Bullets go straigh for now
        //ray.dir += spread.Normalized() * LCG().Float(0.0f, 0.2f);

        Hit hit;
        Physics::Raycast(hit, ray, mAttackRange);

        if (hit.IsValid())
        {
            if (hit.mGameObject->GetTag() == "Enemy")
            {
                LOG("Enemy %s has been hit with MachineGun at distance: %f", hit.mGameObject->GetName().c_str(), hit.mDistance);
                Enemy* enemy = reinterpret_cast<Enemy*>(((ScriptComponent*)hit.mGameObject->GetComponentInParent(ComponentType::SCRIPT))->GetScriptInstance());
                if (enemy)
                {
                    enemy->TakeDamage(mDamage);
                }
            }
        }

        //PARTICLES

        if (mFire)
        {
            mFire->SetEnabled(false);
            mFire->SetEnabled(true);
            mFire->SetPosition(ray.pos + GameManager::GetInstance()->GetPlayer()->GetFront());
        }

        if (GameManager::GetInstance()->GetPoolManager())
        {
            GameObject* bullet = GameManager::GetInstance()->GetPoolManager()->Spawn(PoolType::BULLET);
            Bullet* bulletScript = reinterpret_cast<Bullet*>(reinterpret_cast<ScriptComponent*>(bullet->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());

            ColorGradient gradient;
            gradient.AddColorGradientMark(0.1f, float4(0.686f, 0.0f, 1.0f, 1.0f));
            gradient.AddColorGradientMark(0.6f, float4(0.0f, 0.0f, 1.0f, 1.0f));


            bulletScript->Init(ray.pos, ray.dir, 1.0f, 1.0f, &gradient);
        }
    }
}

void Machinegun::Exit()
{
    mFirstShoot = true;
}

void Machinegun::Reload() 
{
    mCurrentAmmo = mMaxAmmo;
    GameManager::GetInstance()->GetHud()->SetAmmo(mCurrentAmmo);
}

void Machinegun::PlayHitSound()
{
    GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::GUNFIRE, GameManager::GetInstance()->GetPlayer()->GetPosition());
}

bool Machinegun::Delay(float delay)
{
    mTimePassed += App->GetDt();

    if (mTimePassed >= delay)
    {
        mTimePassed = 0;
        return true;
    }
    else return false;
}
