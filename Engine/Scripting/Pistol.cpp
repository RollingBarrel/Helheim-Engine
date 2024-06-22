#include "Pistol.h"
#include "Application.h"

#include "ModuleScene.h"

#include "GameObject.h"
#include "ScriptComponent.h"

#include "Physics.h"
#include "Geometry/Ray.h"

#include "GameManager.h"
#include "PoolManager.h"
#include "HudController.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Bat.h"
#include "TrailComponent.h"

#include <map>

Pistol::Pistol() : RangeWeapon()
{
    mCurrentAmmo = 16;
    mMaxAmmo = 16;
    mDamage = 2.0f;
    mAttackDuration = 0.0f;
    mAttackCooldown = 0.2f;
}

Pistol::~Pistol()
{
}

void Pistol::Enter()
{
}

void Pistol::Attack(float time)
{
    LOG("Pistol Attack");
    GameObject* bullet = nullptr;
    if (mCurrentAmmo > 0) 
    {
        mCurrentAmmo--;
        LOG("Bullets: %i", mCurrentAmmo);
    }
    
    GameManager::GetInstance()->GetHud()->SetAmmo(mCurrentAmmo);

    std::multiset<Hit> hits;

    Ray ray;
    ray.pos = GameManager::GetInstance()->GetPlayer()->GetPosition();
    ray.pos.y++;
    ray.dir = GameManager::GetInstance()->GetPlayer()->GetFront();

    float distance = 100.0f;
    Physics::Raycast(hits, ray, distance);

    if (!hits.empty())
    {
        for (const Hit& hit : hits)
        {
            if (hit.mGameObject->GetTag() == "Enemy")
            {
                LOG("Enemy %s has been hit at distance: %f", hits.begin()->mGameObject->GetName().c_str(), hits.begin()->mDistance);

                Enemy* enemy = reinterpret_cast<Enemy*>(((ScriptComponent*)hit.mGameObject->GetComponentInParent(ComponentType::SCRIPT))->GetScriptInstance());
                if (enemy)
                {
                    enemy->TakeDamage(mDamage);
                }
            }
        }
    }

    //PARTICLES
    if (GameManager::GetInstance()->GetPoolManager())
    {
        GameObject* bullet = GameManager::GetInstance()->GetPoolManager()->Spawn(PoolType::BULLET);
        Bullet* bulletScript = reinterpret_cast<Bullet*>(reinterpret_cast<ScriptComponent*>(bullet->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
        ColorGradient gradient;
        gradient.AddColorGradientMark(0.1f, float4(0.0f, 1.0f, 0.0f, 1.0f));
        
        bulletScript->Init(ray.pos, ray.dir, 1.0f, 1.0f, &gradient);
    }
}

void Pistol::Reload()
{
    mCurrentAmmo = mMaxAmmo;
    GameManager::GetInstance()->GetHud()->SetAmmo(mCurrentAmmo);
}

void Pistol::Exit()
{

}
