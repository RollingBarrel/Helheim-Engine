#include "MeleeWeapon.h"
#include "TrailComponent.h"
#include "BoxColliderComponent.h"
#include "GameObject.h"
#include "Enemy.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ScriptComponent.h"
#include "GameManager.h"
#include "PlayerController.h"


MeleeWeapon::MeleeWeapon(BoxColliderComponent* collider, TrailComponent* trail) : Weapon()
{
	mType = WeaponType::MELEE;
    mCollider = collider;

    assert(mCollider);
    mCollider->AddCollisionEventHandler(
        CollisionEventType::ON_COLLISION_ENTER,
        new std::function<void(CollisionData*)>(std::bind(&MeleeWeapon::OnCollisionEnter, this, std::placeholders::_1))
    );
    mTrail = trail;
}

MeleeWeapon::~MeleeWeapon()
{
}

float MeleeWeapon::GetAttackDuration()
{
    switch (std::max(mComboStep, mNextComboStep))
    {
    case 1: mAttackDuration = mCombo1st; break;
    case 2: mAttackDuration = mCombo1st + mCombo2nd; break;
    case 3: mAttackDuration = mCombo1st + mCombo2nd + mComboEnd; break;
    default: LOG("Should not happend?")break;
    }
    return mAttackDuration;
}

void MeleeWeapon::IncreaseComboStep()
{
    mNextComboStep = mComboStep + 1;
}

void MeleeWeapon::Enter()
{
    if (mCollider)
    {
        mCollider->SetEnable(true);
    }
    if (mTrail) 
    {
        mTrail->SetEnable(true);
    }
    LOG("MELEE ENTER");
    mColliderAtivated = false;
    mComboStep = 1;
    mNextComboStep = 1;
    mHasHitted = false;
    // animation hit 1
}

void MeleeWeapon::Attack(float time)
{
    // Deactivates the collider 1 frame after the collider gets activated
    if (time > mCombo1st * mHitTime and
        mComboStep == 1 and mColliderAtivated)
    {
        LOG("MELEE: Finish 1    Time: %f", time);
        mColliderAtivated = false;
        mHasHitted = true;
    }
    else if (time > mCombo1st + mCombo2nd * mHitTime and
        mComboStep == 2 and mColliderAtivated)
    {
        LOG("MELEE: Finish 2    Time: %f", time);
        mColliderAtivated = false;
        mHasHitted = true;
    }
    else if (time > mCombo1st + mCombo2nd + mComboEnd * mHitTime and
        mComboStep == 3 and mColliderAtivated)
    {
        LOG("MELEE: Finish 3    Time: %f", time);
        mColliderAtivated = false;
        mHasHitted = true;
    }
    
    // Activates the collider damage effect
    if (time > mCombo1st * mHitTime and 
        mComboStep == 1 and !mColliderAtivated and !mHasHitted)
    {
        LOG("MELEE: Start 1    Time: %f", time);
        mColliderAtivated = true;
    }
    else if (time > mCombo1st + mCombo2nd * mHitTime and 
        mComboStep == 2 and !mColliderAtivated and !mHasHitted)
    {
        LOG("MELEE: Start 2    Time: %f", time);
        mColliderAtivated = true;
    }
    else if (time > mCombo1st + mCombo2nd + mComboEnd * mHitTime and 
        mComboStep == 3 and !mColliderAtivated and !mHasHitted)
    {
        LOG("MELEE: Start 3    Time: %f", time);
        mColliderAtivated = true;
    }

    // Increase the combo step after activating the collider effect
    if (time > mCombo1st and mNextComboStep == 2 and mComboStep == 1 and mHasHitted)
    {
        LOG("MELEE: Combo 2    Time: %f", time);
        mComboStep = 2;
        mHasHitted = false;
        // animation hit 2
        //GameManager::GetInstance()->GetPlayerController()->SetAnimation(trigger, transitionTime);

    }
    else if (time > mCombo1st + mCombo2nd and mNextComboStep == 3 and mComboStep == 2 and mHasHitted)
    {
        LOG("MELEE: Combo 3    Time: %f", time);
        mComboStep = 3;
        mHasHitted = false;
        // animation hit 3

    }
}

void MeleeWeapon::Exit()
{
    if (mCollider)
    {
        mCollider->SetEnable(false);
    }
    if (mTrail) mTrail->SetEnable(false);
    mColliderAtivated = false;

}

void MeleeWeapon::OnCollisionEnter(CollisionData* collisionData)
{
    // pop particles on collision
    if (collisionData->collidedWith->GetTag().compare("Enemy") == 0 and mColliderAtivated)
    {
        DealDamage(collisionData->collidedWith);
        LOG("Colliding with melee!");

    }
}

void MeleeWeapon::DealDamage(GameObject* enemy)
{
    Enemy* enemyScript = reinterpret_cast<Enemy*>(reinterpret_cast<ScriptComponent*>(enemy->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
    if (enemyScript) {
        enemyScript->TakeDamage(mDamage);
        //enemyScript->PushBack(); // TODO: Its best to push it back with CollisionData normal
    }
}