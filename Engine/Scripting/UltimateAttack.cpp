#include "UltimateAttack.h"
#include "Application.h"
#include "GameObject.h"
#include "Enemy.h"

#include "ScriptComponent.h"
#include "BoxColliderComponent.h"

CREATE(UltimateAttack)
{
    CLASS(owner);
    MEMBER(MemberType::GAMEOBJECT, mLaserGO);
    END_CREATE;
}
UltimateAttack::UltimateAttack(GameObject* owner) : Script(owner)
{
}

UltimateAttack::~UltimateAttack()
{
}

void UltimateAttack::Start()
{
    mCollider = static_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
    if (mCollider)
    {
        mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&UltimateAttack::OnCollisionEnter, this, std::placeholders::_1)));
    }
}

void UltimateAttack::Update()
{
}

void UltimateAttack::OnCollisionEnter(CollisionData* collisionData)
{
    GameObject* collisionGO = collisionData->collidedWith;
    if (collisionGO->GetTag() == "Enemy")
    {
        Enemy* enemyScript = static_cast<Enemy*>(static_cast<ScriptComponent*>(collisionGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
        if (enemyScript)
        {
            if (mDamageTimer.Delay(mInterval)) 
            {
                enemyScript->TakeDamage(mDamageTick);
                //TODO: Slow enemies 
                LOG("Ultimate tick")
            }
        }
    }
}
