#include "Machinegun.h"
#include "Application.h"
#include "GameManager.h"
#include "HudController.h"
#include "PoolManager.h"
#include "AudioManager.h"
#include "RayCastBullet.h"
#include "Enemy.h"

#include "GameObject.h"
#include "ScriptComponent.h"
#include "TrailComponent.h"
#include "Physics.h"

#include "Application.h"
#include "ModuleScene.h"

#include "Geometry/Ray.h"
#include "Algorithm/Random/LCG.h"
#include "PlayerController.h"

#include "ModuleInput.h"

Machinegun::Machinegun()
{
	mDamage = 4.0f;
	mAttackDuration = 0.3f;
    mAttackCooldown = 0.3f;
	mAttackRange = 25.0f;
    mNumBullets = 3;

    mBulletSpeed = 60.0f;

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
    //CONTROLLER VIBRATION
    App->GetInput()->SetGameControllerRumble(40000, 0, 100);
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
        static_cast<PlayerController*>(static_cast<ScriptComponent*>(GameManager::GetInstance()->GetPlayer()->GetComponent(ComponentType::SCRIPT))->GetScriptInstance())->UseEnergy(mNumBullets);
        //Audio
        if (GameManager::GetInstance()->GetAudio())
        {
            PlayHitSound();
        }

        //Shoot Logic
        Ray ray;
        ray.pos = GameManager::GetInstance()->GetPlayer()->GetWorldPosition();
        ray.pos.y++;
        ray.dir = GameManager::GetInstance()->GetPlayer()->GetFront();

        //Bullets go straigh for now
        //ray.dir += spread.Normalized() * LCG().Float(0.0f, 0.2f);

        Hit hit;
        std::vector<std::string> ignoreTags = { "Bullet", "BattleArea", "Trap", "Drop" };
        Physics::Raycast(hit, ray, mAttackRange, &ignoreTags);

        if (hit.IsValid())
        {
            if (hit.mGameObject->GetTag() == "Enemy")
            {
                LOG("Enemy %s has been hit with MachineGun at distance: %f", hit.mGameObject->GetName().c_str(), hit.mDistance);
                Enemy* enemy = static_cast<Enemy*>(((ScriptComponent*)hit.mGameObject->GetComponentInParent(ComponentType::SCRIPT))->GetScriptInstance());
                if (enemy)
                {
                    enemy->TakeDamage(mDamage * GameManager::GetInstance()->GetPlayerController()->GetDamageModifier());
                }
            }
        }

        //PARTICLES

        if (mFire)
        {
            mFire->SetEnabled(false);
            mFire->SetEnabled(true);
            mFire->SetLocalPosition(ray.pos + GameManager::GetInstance()->GetPlayer()->GetFront());
        }

        if (GameManager::GetInstance()->GetPoolManager())
        {
            GameObject* bullet = GameManager::GetInstance()->GetPoolManager()->Spawn(PoolType::BULLET);
            RayCastBullet* bulletScript = static_cast<RayCastBullet*>(static_cast<ScriptComponent*>(bullet->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());

            ColorGradient gradient;
            gradient.AddColorGradientMark(0.1f, float4(0.686f, 0.0f, 1.0f, 1.0f));
            gradient.AddColorGradientMark(0.6f, float4(0.0f, 0.0f, 1.0f, 1.0f));
            bullet->SetEnabled(false);
            if (hit.IsValid())
            {
                bulletScript->Init(ray.pos, hit.mHitPoint, mBulletSpeed, mBulletSize, true, &gradient);
            }
            else
            {
                bulletScript->Init(ray.pos, ray.pos + ray.dir.Mul(mAttackRange), mBulletSpeed, mBulletSize, false, &gradient);
            }
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
    GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::GUNFIRE, GameManager::GetInstance()->GetPlayer()->GetWorldPosition());
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
