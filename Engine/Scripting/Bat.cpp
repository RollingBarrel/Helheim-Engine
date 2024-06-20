#include "Bat.h"
#include "float3.h"
#include <vector>
#include "TrailComponent.h"
#include "GameObject.h"
#include "Enemy.h"
#include "Application.h"
#include "ModuleScene.h"

Bat::Bat() : MeleeWeapon()
{
    mAttackTime = 3.0f;
    mDamage = 2.0f;
    mComboStep = 0;
    mComboMilestone1 = 1.0f;
    mComboMilestone2 = 2.0f;
    mComboDuration = 3.0f;
}

Bat::~Bat()
{
}

void Bat::Enter()
{
    //mTrail->Enable();
    mComboStep = 1;
}

void Bat::Update(float deltaTime)
{


}

void Bat::DealDamage(GameObject* enemy) 
{
    // This must generate the box colider, detect the gameObjects that are inside, 
    // apply damage to the enemies and 
    // pop particles from the game object

    Enemy* enemyScript = dynamic_cast<Enemy*>(enemy->GetComponent(ComponentType::SCRIPT));
    if (enemyScript) {
        enemyScript->TakeDamage(mDamage);
        enemyScript->PushBack();
    }
}


void Bat::Attack(float time)
{
    // - Calculate hitbox and intersect it with every object to spawn sparks
    // - Damage to enemies inside the hitbox
    // LOG("MELEE ATTACK!!")

    if (time > mComboMilestone1 and mComboStep == 1)
    {
        LOG("MELEE ATTACK 1!!")

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

        // DealDamage();
    }

}

void Bat::Exit()
{
    //mTrail->Disable();
}

