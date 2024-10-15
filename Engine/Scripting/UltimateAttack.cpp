#include "UltimateAttack.h"
#include "Application.h"
#include "GameObject.h"
#include "Enemy.h"
#include "MathFunc.h"
#include "ModuleScene.h"


#include "Physics.h"
#include "Geometry/Ray.h"
#include "MathFunc.h"

#include "ScriptComponent.h"
#include "BoxColliderComponent.h"

CREATE(UltimateAttack)
{
    CLASS(owner);
    MEMBER(MemberType::GAMEOBJECT, mLinesGO);
    MEMBER(MemberType::GAMEOBJECT, mFinalPoint1);
    MEMBER(MemberType::GAMEOBJECT, mFinalPoint2);
    MEMBER(MemberType::GAMEOBJECT, mFinalPoint3);
    
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
    if (!mExpansionTimer.DelayWithoutReset(3.8f)) 
    {
        SetLength(100.0f, 25.0f);
        SetFinalPoint();
    }
    else
    {
        SetLength(10.0f,15.0f);/*
        if (mExpansionTimer.GetTimePassed()>4.0f) mExpansionTimer.Reset();*/
    }
    
}

void UltimateAttack::SetLength(float targetPercent, float speed)
{
    if (mLinesGO)
    {
        float3 scale = mLinesGO->GetLocalScale();
        float length = scale.z;
        length = Lerp(length, 1.0f, App->GetDt()*speed);
        mLengthPercent = Lerp(mLengthPercent, targetPercent, App->GetDt() * speed);
        scale.z = length * (mLengthPercent/100.0f);
        mLinesGO->SetLocalScale(scale);
    }

    //else mGameObject->SetLocalScale(float3(1.0, 1.0, 1.0));a
}

void UltimateAttack::SetFinalPoint()
{
    float3 currentFinalpoint = mFinalPoint1->GetWorldPosition();
    Hit hit;
    Ray ray;
    ray.dir = mGameObject->GetFront();
    ray.pos = mGameObject->GetWorldPosition();
    std::vector<std::string> ignoreTags = { "Bullet", "BattleArea", "Trap", "Drop", "Bridge", "DoorArea", "Collectible","Player","Enemy" };
    Physics::Raycast(hit, ray, 35.0f, &ignoreTags);
    if (hit.IsValid()) {

        /*currentFinalpoint.x = Lerp(currentFinalpoint.x, hit.mHitPoint.x, App->GetDt());
        currentFinalpoint.z = Lerp(currentFinalpoint.z, hit.mHitPoint.z, App->GetDt());*/
        currentFinalpoint.x = hit.mHitPoint.x;
        currentFinalpoint.z = hit.mHitPoint.z;
        mFinalPoint1->SetWorldPosition(currentFinalpoint);
        mFinalPoint2->SetWorldPosition(currentFinalpoint);
        mFinalPoint3->SetWorldPosition(currentFinalpoint);
    }
}

void UltimateAttack::OnCollisionEnter(CollisionData* collisionData)
{
    GameObject* collisionGO = collisionData->collidedWith;
    Hit hit;
    Ray ray;
    ray.dir = mGameObject->GetFront();
    ray.pos = mGameObject->GetWorldPosition();


    if (collisionGO->GetTag() == "Enemy")
    {
        Enemy* enemyScript = static_cast<Enemy*>(static_cast<ScriptComponent*>(collisionGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
        if (enemyScript)
        {
            if (mDamageTimer.Delay(mInterval)) 
            {
                enemyScript->ActivateUltVFX();
                enemyScript->TakeDamage(mDamageTick);
                //TODO: Slow enemies 
                LOG("Ultimate tick")
            }
        }
    }

    
}
