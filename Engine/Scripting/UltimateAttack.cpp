#include "UltimateAttack.h"
#include "Application.h"
#include "GameObject.h"
#include "Enemy.h"
#include "MathFunc.h"

#include "Physics.h"
#include "Geometry/Ray.h"

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
    else if (collisionGO->GetTag() != "Player" && collisionGO->GetTag() != "Drop" && collisionGO->GetTag() != "Trap")
    {
         //float distance = Distance(float2(collisionGO->GetWorldPosition().x, collisionGO->GetWorldPosition().z), float2(mGameObject->GetWorldPosition().x, mGameObject->GetWorldPosition().z)) / 10;
         //Clamp01(distance);
         float3 currentScale = mGameObject->GetLocalScale();
         ////if (distance < currentScale.z)
         //   mGameObject->SetLocalScale(float3(currentScale.x, currentScale.y, distance));
        Hit hit;
        Ray ray;
        ray.dir = mGameObject->GetFront();
        ray.pos = mGameObject->GetWorldPosition();

        Physics::Raycast(hit, ray, 10.0f);  
        if (hit.IsValid()) {
            float distance = Distance(float2(hit.mHitPoint.x, hit.mHitPoint.z), float2(mGameObject->GetWorldPosition().x, mGameObject->GetWorldPosition().z)) / 10;
            mGameObject->SetLocalScale(float3(currentScale.x, currentScale.y, distance));
        }
    }
    else mGameObject->SetLocalScale(float3(1.0, 1.0, 1.0));
}
