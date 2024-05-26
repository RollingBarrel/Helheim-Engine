#include "Pistol.h"
#include "Application.h"

#include "ModuleScene.h"

#include "GameObject.h"
#include "ScriptComponent.h"

#include "Physics.h"
#include "Geometry/Ray.h"

#include "Enemy.h"
#include "Bullet.h"

#include <map>

CREATE(Pistol)
{
    CLASS(owner);
    //SEPARATOR("STATS");
    MEMBER(MemberType::GAMEOBJECT, mShootPoint);
   
    END_CREATE;
}

void Pistol::BasicAttack()
{

    GameObject* bullet = nullptr;
    if (Bullet::GetNumBullets() < 10)
    {
        bullet = App->GetScene()->InstantiatePrefab("PistolBullet.prfb");
    }

    if (bullet != nullptr)
    {
        if (mShootPoint)
        {
            LOG("%f, %f, %f", mShootPoint->GetWorldPosition().x, mShootPoint->GetWorldPosition().y, mShootPoint->GetWorldPosition().z);
            bullet->SetPosition(mShootPoint->GetWorldPosition() + float3(0.0f, 1.0f, 0.0f));
        }
        bullet->SetRotation(mGameObject->GetWorldRotation());
    }

    std::map<float, Hit> hits;

    Ray ray;
    ray.pos = mGameObject->GetPosition();
    ray.pos.y++;
    ray.dir = mGameObject->GetFront();

    float distance = 100.0f;
    hits = Physics::Raycast(&ray);

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
    }
}

void Pistol::SpecialAttack()
{
}
