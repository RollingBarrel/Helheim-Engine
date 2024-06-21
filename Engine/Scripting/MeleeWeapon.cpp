#include "MeleeWeapon.h"
#include "TrailComponent.h"
#include "BoxColliderComponent.h"
#include "GameObject.h"
#include "Enemy.h"
#include "Application.h"


MeleeWeapon::MeleeWeapon(BoxColliderComponent* collider) : Weapon()
{
	mType = WeaponType::MELEE;
    mCollider = collider;

    assert(mCollider);
    mCollider->AddCollisionEventHandler(
        CollisionEventType::ON_COLLISION_ENTER,
        new std::function<void(CollisionData*)>(std::bind(&MeleeWeapon::OnCollisionEnter, this, std::placeholders::_1))
    );
    mCollider->SetEnable(false);
}

MeleeWeapon::~MeleeWeapon()
{
}

float MeleeWeapon::GetAttackTime()
{
    switch (mComboStep)
    {
    case 1: mAttackTime = mCombo1st; break;
    case 2: mAttackTime = mCombo1st + mCombo2nd; break;
    case 3: mAttackTime = mCombo1st + mCombo2nd + mComboEnd; break;
    default: break;
    }
    return mAttackTime;
}

void MeleeWeapon::IncreaseComboStep()
{
    mNextComboStep = mComboStep + 1;
}

void MeleeWeapon::Enter()
{
    //mTrail->Enable();
    if (mCollider) mCollider->SetEnable(true);
    mComboStep = 1;
}

void MeleeWeapon::Attack(float time)
{
    //if (mCollider->IsEnabled()) mCollider->SetEnable(false);

    if (time > mCombo1st * mHitTime and 
        mComboStep == 1 and !mColliderAtivated)
    {
        LOG("MELEE: ATTACK 1!!");
        //mCollider->SetEnable(true);
        mColliderAtivated = true;
        if (mNextComboStep == 2)
        {
            mComboStep = 2;
            mColliderAtivated = false;
            // mAnimationComponent->SendTrigger("tMelee", 0.2f);
        }
    }
    else if (time > mCombo1st + mCombo2nd * mHitTime and 
        mComboStep == 2 and !mColliderAtivated)
    {
        LOG("MELEE ATTACK 2!!");
        //mCollider->SetEnable(true);
        mColliderAtivated = true;
        // DealDamage();
        if (mNextComboStep == 3)
        {
            mComboStep = 3;
            // mAnimationComponent->SendTrigger("tMelee", 0.2f);
        }
    }
    else if (time > mCombo1st + mCombo2nd + mComboEnd * mHitTime and 
        mComboStep == 3 and !mColliderAtivated)
    {
        LOG("MELEE ATTACK 3!!");
        //mCollider->SetEnable(true);
        mColliderAtivated = true;
        // DealDamage();
    }

}

void MeleeWeapon::Exit()
{
    //mTrail->Disable();
    if (mCollider) mCollider->SetEnable(false);
    mColliderAtivated = false;

}

void MeleeWeapon::OnCollisionEnter(CollisionData* collisionData)
{
    if (collisionData->collidedWith->GetTag().compare("Enemy") == 0)
    {
        DealDamage(collisionData->collidedWith);
        LOG("Colliding with melee!");

    }
}

void MeleeWeapon::DealDamage(GameObject* enemy)
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