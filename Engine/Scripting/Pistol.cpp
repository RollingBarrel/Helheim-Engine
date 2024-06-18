#include "Pistol.h"
#include "Application.h"

#include "ModuleScene.h"

#include "GameObject.h"
#include "ScriptComponent.h"

#include "Physics.h"
#include "Geometry/Ray.h"

#include "Enemy.h"
#include "Bullet.h"
#include "HudController.h"
#include "GameManager.h"

#include <map>
#include "Bat.h"

Pistol::Pistol() : RangeWeapon()
{
    mCurrentAmmo = 16;
    mMaxAmmo = 16;

    mDamage = 10.0f;
    mAttackRate = 1.0f;
}

Pistol::~Pistol()
{
}

void Pistol::Enter()
{
}

void Pistol::Attack()
{
    GameObject* bullet = nullptr;
    if (mCurrentAmmo > 0) 
    {
       //bullet = App->GetScene()->InstantiatePrefab("PistolBullet.prfb");
        mCurrentAmmo--;
        LOG("Bullets: %i", mCurrentAmmo);
    }
    

    //mPlayerController->PlayOneShot("Shot");
    //mAnimationComponent->SendTrigger("tShooting", 0.2f);
    GameManager::GetInstance()->GetHud()->SetAmmo(mCurrentAmmo);

    if (bullet != nullptr)
    {
        /*bullet->SetPosition(mPlayerCharacter->GetPosition());

        mShootPoint->SetEnabled(false);
        mShootPoint->SetEnabled(true); // Reset particles
        
        bullet->SetRotation(mGameObject->GetWorldRotation());*/
    }

    std::multiset<Hit> hits;

    Ray ray;
    ray.pos = GameManager::GetInstance()->GetPlayer()->GetPosition();
    ray.pos.y++;
    ray.dir = GameManager::GetInstance()->GetPlayer()->GetFront();

    float distance = 100.0f;
    Physics::Raycast(hits, ray, distance); // THIS IS THE OLD RAYCAST

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
}

//void Pistol::Reload()
//{
//    mCurrentAmmo = mMaxAmmo;
//    GameManager::GetInstance()->GetHud()->SetAmmo(mCurrentAmmo);
//}

void Pistol::Exit()
{

}
