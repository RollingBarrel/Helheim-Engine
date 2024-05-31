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

#include <map>

CREATE(Pistol)
{
    CLASS(owner);
    //SEPARATOR("STATS");
    MEMBER(MemberType::GAMEOBJECT, mShootPoint);

    SEPARATOR("HUD");
    MEMBER(MemberType::GAMEOBJECT, mHudControllerGO);

    END_CREATE;
}

void Pistol::Start()
{
    if (mHudControllerGO)
    {
        ScriptComponent* script = static_cast<ScriptComponent*>(mHudControllerGO->GetComponent(ComponentType::SCRIPT));
        mHudController = static_cast<HudController*>(script->GetScriptInstance());
    }
}

void Pistol::BasicAttack()
{
    GameObject* bullet = nullptr;
    if (mCurrentAmmo > 0) {
        bullet = App->GetScene()->InstantiatePrefab("PistolBullet.prfb");
        mCurrentAmmo--;
    }
    else {
        mCurrentAmmo = mMaxAmmo;
    }
    
    mHudController->SetAmmo(mCurrentAmmo);

    if (bullet != nullptr)
    {
        if (mShootPoint)
        {
            LOG("%f, %f, %f", mShootPoint->GetWorldPosition().x, mShootPoint->GetWorldPosition().y, mShootPoint->GetWorldPosition().z);
            bullet->SetPosition(mShootPoint->GetWorldPosition());

            mShootPoint->SetEnabled(false);
            mShootPoint->SetEnabled(true); // Reset particles
        }
        bullet->SetRotation(mGameObject->GetWorldRotation());
    }

    /*std::map<float, Hit> hits;

    Ray ray;
    ray.pos = mGameObject->GetPosition();
    ray.pos.y++;
    ray.dir = mGameObject->GetFront();

    float distance = 100.0f;
    Physics::Raycast(ray, hits);

    if (!hits.empty())
    {
        for (const std::pair<float, Hit>& hit : hits)
        {
            if (hit.second.mGameObject->GetTag()->GetName() == "Enemy")
            {
                LOG("Enemy %s has been hit at distance: %f", hits.begin()->second.mGameObject->GetName().c_str(), hits.begin()->first);

                Enemy* enemy = reinterpret_cast<Enemy*>(((ScriptComponent*)hit.second.mGameObject->GetComponentInParent(ComponentType::SCRIPT))->GetScriptInstance());
                if (enemy)
                {
                    enemy->TakeDamage(mDamage);
                }
            }
        }
    }*/
}

void Pistol::SpecialAttack()
{
}
