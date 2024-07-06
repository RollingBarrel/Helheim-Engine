#include "EnemyCreatureMelee.h"


CREATE(EnemyCreatureMelee)
{
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::FLOAT, mMaxHealth);
    MEMBER(MemberType::FLOAT, mSpeed);
    MEMBER(MemberType::FLOAT, mRotationSpeed);
    MEMBER(MemberType::INT, mShieldDropRate);
    MEMBER(MemberType::FLOAT, mChaseDelay);
    MEMBER(MemberType::FLOAT, mMeleeAttackCoolDown);
    MEMBER(MemberType::FLOAT, mMeleeDistance);
    MEMBER(MemberType::FLOAT, mMeeleDamage);

    END_CREATE;
}



EnemyCreatureMelee::EnemyCreatureMelee(GameObject* owner) : Enemy(owner)
{
}

void EnemyCreatureMelee::Start()
{
    Enemy::Start();
}

void EnemyCreatureMelee::Update()
{
    Enemy::Update();
}

void EnemyCreatureMelee::Idle()
{
}

void EnemyCreatureMelee::Chase()
{
}

void EnemyCreatureMelee::Attack()
{
}

bool EnemyCreatureMelee::IsMoving()
{
    return (mCurrentState == EnemyState::CHASE);
}

void EnemyCreatureMelee::MeleeAttack()
{
}

void EnemyCreatureMelee::Death()
{
}

void EnemyCreatureMelee::Init()
{
}

void EnemyCreatureMelee::OnCollisionEnter(CollisionData* collisionData)
{
}

void EnemyCreatureMelee::PlayStepAudio()
{
}

void EnemyCreatureMelee::PlayMeleeAudio()
{
}
