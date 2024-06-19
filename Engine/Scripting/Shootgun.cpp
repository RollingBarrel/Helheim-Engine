#include "Shootgun.h"
#include "GameObject.h"
#include "GameManager.h"
#include "HudController.h"
#include <Physics.h>
#include "Geometry/Ray.h"
#include <Enemy.h>
#include <ScriptComponent.h>

Shootgun::Shootgun()
{
    mDamage = 0.1f;
}

Shootgun::~Shootgun()
{
}

void Shootgun::Enter()
{
   
}

void Shootgun::Attack()
{
    unsigned int numBullets = 10;


    GameObject* bullet = nullptr;

    if (mCurrentAmmo > 0)
    {
        //bullet = App->GetScene()->InstantiatePrefab("PistolBullet.prfb");
        mCurrentAmmo--;
    }
    else {
        mCurrentAmmo = mMaxAmmo;
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

    std::vector<Hit> hits;
    hits.reserve(numBullets);


    Ray ray;
    ray.pos = GameManager::GetInstance()->GetPlayer()->GetPosition();
    ray.pos.y++;
    ray.dir = GameManager::GetInstance()->GetPlayer()->GetFront();

    float distance = 100.0f;
    for (unsigned int i = 0; i < numBullets; ++i)
    {
        Hit hit;
        Physics::Raycast(hit, ray, distance);

        if (hit.IsValid())
        {
            if (hit.mGameObject->GetTag() == "Enemy")
            {
                LOG("Enemy %s has been hit with ShotGun at distance: %f", hit.mGameObject->GetName().c_str(), hit.mDistance);
                Enemy* enemy = reinterpret_cast<Enemy*>(((ScriptComponent*)hit.mGameObject->GetComponentInParent(ComponentType::SCRIPT))->GetScriptInstance());
                if (enemy)
                {
                    enemy->TakeDamage(mDamage);
                }
            }
        }
    }
    
}

void Shootgun::Exit()
{
}
