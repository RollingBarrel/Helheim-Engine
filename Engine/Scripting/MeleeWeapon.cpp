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
#include "Hammer.h"
#include "Katana.h"

MeleeWeapon::MeleeWeapon(BoxColliderComponent* collider, TrailComponent* trail) : Weapon()
{
    mType = WeaponType::MELEE;
    mCollider = collider;

    if (mCollider) 
    {
        mCollider->AddCollisionEventHandler(
            CollisionEventType::ON_COLLISION_ENTER,
            new std::function<void(CollisionData*)>(std::bind(&MeleeWeapon::OnCollisionEnter, this, std::placeholders::_1))
        );
    }
    mTrail = trail;
    if (mTrail) mTrail->SetEnable(false);

    mPlayerGO = GameManager::GetInstance()->GetPlayer();
    mPlayerController = GameManager::GetInstance()->GetPlayerController();
    mLastComboStartTime = 0.0f;
    mMovingForward = false;

    mAttackDuration = 0.5f;
}

MeleeWeapon::~MeleeWeapon()
{
}

float MeleeWeapon::GetAttackDuration()
{
    /*switch (std::max(mComboStep, mNextComboStep))
    {
    case 1: mAttackDuration = mCombo1st; break;
    case 2: mAttackDuration = mCombo1st + mCombo2nd; break;
    case 3: mAttackDuration = mCombo1st + mCombo2nd + mComboEnd; break;
    default: LOG("Should not happen?") break;
    }*/
    return mAttackDuration;
}

void MeleeWeapon::IncreaseComboStep()
{
    mNextComboStep = mComboStep + 1;
    if (mNextComboStep < 1 || mNextComboStep > 3) 
    {
        LOG("Invalid combo step detected: %d", mNextComboStep);
        mNextComboStep = 1; 
    }
}

void MeleeWeapon::Enter()
{
    if (mCollider)
    {
        mCollider->SetEnable(true);
        mCollider->GetOwner()->SetEnabled(true);
    }
    if (mTrail) 
    {
        mTrail->SetEnable(true);
    }
    //LOG("MELEE ENTER");


    //mColliderAtivated = false;
    //mComboStep = 1;
    //mNextComboStep = 1;
    //mHasHitted = false;

    //mPlayerController->SetSpineAnimation("tAttackMelee", 0.9f);
    // 
    //CONTROLLER VIBRATION
    App->GetInput()->SetGameControllerRumble(0, 45000, 100);
}

void MeleeWeapon::Attack(float time)
{
    // Deactivates the collider 1 frame after the collider gets activated
    /*if (time > mCombo1st * mHitTime &&
        mComboStep == 1 && mColliderAtivated)
    {
        LOG("MELEE: Finish 1    Time: %f", time);
        mColliderAtivated = false;
        mHasHitted = true;
    }
    else if (time > mCombo1st + mCombo2nd * mHitTime &&
        mComboStep == 2 && mColliderAtivated)
    {
        LOG("MELEE: Finish 2    Time: %f", time);
        mColliderAtivated = false;
        mHasHitted = true;
    }
    else if (time > mCombo1st + mCombo2nd + mComboEnd * mHitTime &&
        mComboStep == 3 && mColliderAtivated)
    {
        LOG("MELEE: Finish 3    Time: %f", time);
        mColliderAtivated = false;
        mHasHitted = true;
    }

    // Activates the collider damage effect
    if (time > mCombo1st * mHitTime &&
        mComboStep == 1 && !mColliderAtivated && !mHasHitted)
    {
        LOG("MELEE: Start 1    Time: %f", time);
        mColliderAtivated = true;
    }
    else if (time > mCombo1st + mCombo2nd * mHitTime &&
        mComboStep == 2 && !mColliderAtivated && !mHasHitted)
    {
        LOG("MELEE: Start 2    Time: %f", time);
        mColliderAtivated = true;
    }
    else if (time > mCombo1st + mCombo2nd + mComboEnd * mHitTime &&
        mComboStep == 3 && !mColliderAtivated && !mHasHitted)
    {
        LOG("MELEE: Start 3    Time: %f", time);
        mColliderAtivated = true;
    }

    // Increase the combo step after activating the collider effect
    if (time > mCombo1st && mNextComboStep == 2 && mComboStep == 1 && mHasHitted)
    {
        LOG("MELEE: Combo 2    Time: %f", time);
        mComboStep = 2;
        mHasHitted = false;

        //CONTROLLER VIBRATION
        App->GetInput()->SetGameControllerRumble(0, 45000, 100);

        // animation hit 2
        mPlayerController->SetAnimationSpeed(0.07f);
        mPlayerController->SetSpineAnimation("tAttackMelee", 0.07f);
    }
    else if (time > mCombo1st + mCombo2nd && mNextComboStep == 3 && mComboStep == 2 && mHasHitted)
    {
        LOG("MELEE: Combo 3    Time: %f", time);
        mComboStep = 3;
        mHasHitted = false;
        mLastComboStartTime = time;
        mMovingForward = true;
        // animation hit 3
        mPlayerController->SetAnimationSpeed(0.1f);
        mPlayerController->SetSpineAnimation("tAttackMelee", 0.1f);
        //CONTROLLER VIBRATION
        App->GetInput()->SetGameControllerRumble(0, 45000, 100);
    }

    if (mMovingForward && mComboStep == 3)
    {
        float totalMoveDistance = 0.05f; 
        float moveSpeed = totalMoveDistance / mComboEnd; 

        float3 forward = mPlayerGO->GetFront();
        float moveAmount = moveSpeed * (time - mLastComboStartTime);

        if (moveAmount >= totalMoveDistance)
        {
            moveAmount = totalMoveDistance;
            mMovingForward = false;
            IncreaseComboStep();
        }

        mPlayerGO->SetWorldPosition(mPlayerGO->GetWorldPosition() + forward * moveAmount);
    }*/
}

void MeleeWeapon::Exit()
{
    if (mCollider)
    {
        mCollider->SetEnable(false);
        mCollider->GetOwner()->SetEnabled(false);
    }
    if (mTrail) mTrail->SetEnable(false);
    
    //mColliderAtivated = false;
    //mMovingForward = false; 

}

void MeleeWeapon::OnCollisionEnter(CollisionData* collisionData)
{
    // pop particles on collision

    if (mCollider->IsEnabled()) 
    {
         if (collisionData->collidedWith->GetTag() == "Enemy")
        {
            HitEffect(collisionData->collidedWith);
            LOG("Colliding with enemy!");
        }
    }
}