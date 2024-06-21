#include "Bat.h"
#include "float3.h"
#include <vector>
#include "TrailComponent.h"
#include "BoxColliderComponent.h"
#include "GameObject.h"
#include "Enemy.h"
#include "Application.h"
#include "ModuleScene.h"

Bat::Bat(BoxColliderComponent* collider) : MeleeWeapon()
{
    mAttackTime = 3.0f;
    mDamage = 2.0f;
    mComboStep = 0;
    mComboMilestone1 = 1.0f;
    mComboMilestone2 = 2.0f;
    mComboDuration = 3.0f;
    mCollider = collider;

    // Do it in playerController to pass collider
    //mCollider = reinterpret_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
    if (mCollider)
    {
        mCollider->AddCollisionEventHandler(
            CollisionEventType::ON_COLLISION_ENTER, 
            new std::function<void(CollisionData*)>(std::bind(&Bat::OnCollisionEnter, this, std::placeholders::_1))
        );
    }
    mCollider->SetEnable(false);
}

Bat::~Bat()
{
}

void Bat::Enter()
{
    //mTrail->Enable();
    if(mCollider) mCollider->SetEnable(true);
    mComboStep = 1;
}

void Bat::Update(float deltaTime)
{


}

void Bat::OnCollisionEnter(CollisionData* collisionData)
{
    LOG("Colliding with melee!")
    if (collisionData->collidedWith->GetTag().compare("Enemy"))
    {
        DealDamage(collisionData->collidedWith);
    }
}

void Bat::DealDamage(GameObject* enemy) 
{
    // This must generate the box colider, detect the gameObjects that are inside, 
    // apply damage to the enemies and 
    // pop particles from the game object
     Enemy* enemyScript = dynamic_cast<Enemy*>(enemy->GetComponent(ComponentType::SCRIPT));
    if (enemyScript) {
        enemyScript->TakeDamage(mDamage);
        enemyScript->PushBack(); // TODO: Its best to push it back with CollisionData normal
    }
}


void Bat::Attack(float time)
{
    // - Calculate hitbox and intersect it with every object to spawn sparks
    // - Damage to enemies inside the hitbox
    // LOG("MELEE ATTACK!!")
    if (mCollider->IsEnabled()) mCollider->SetEnable(false);
    LOG("MELEE: %f", time)


    if (time > mComboMilestone1 and mComboStep == 1)
    {
        LOG("MELEE: ATTACK 1!!")
        mCollider->SetEnable(true);
        // DealDamage();
        if (mNextComboStep == 2)
        {
            mComboStep = 2;
            // mAnimationComponent->SendTrigger("tMelee", 0.2f);
        }
    }
    else if (time > mComboMilestone2 and mComboStep == 2)
    {
        LOG("MELEE ATTACK 2!!")
        mCollider->SetEnable(true);
        // DealDamage();
        if (mNextComboStep == 3)
        {
            mComboStep = 3;
            // mAnimationComponent->SendTrigger("tMelee", 0.2f);
        }
    }
    else if (time > mComboDuration and mComboStep == 3)
    {
        LOG("MELEE ATTACK 3!!")
        mCollider->SetEnable(true);
        // DealDamage();
    }

}

void Bat::Exit()
{
    //mTrail->Disable();
    if(mCollider) mCollider->SetEnable(false);

}

