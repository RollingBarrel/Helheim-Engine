#include "EnemyCreatureRange.h"

CREATE(EnemyCreatureRange) {
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::FLOAT, mMaxHealth);
    MEMBER(MemberType::FLOAT, mSpeed);
    MEMBER(MemberType::FLOAT, mRotationSpeed);
    MEMBER(MemberType::FLOAT, mChaseDelay);
    MEMBER(MemberType::FLOAT, mRangeDistance);
    MEMBER(MemberType::FLOAT, mRangeDamage);
    MEMBER(MemberType::FLOAT, mBulletSpeed);
    MEMBER(MemberType::FLOAT, mTimerAttack);
    MEMBER(MemberType::GAMEOBJECT, mBulletOrigin);
    END_CREATE;
}


EnemyCreatureRange::EnemyCreatureRange(GameObject* owner) :Enemy(owner)
{
}

void EnemyCreatureRange::Start()
{
    Enemy::Start();
}

void EnemyCreatureRange::Update()
{
    Enemy::Update();
}

void EnemyCreatureRange::Idle()
{
}

void EnemyCreatureRange::Chase()
{
}

void EnemyCreatureRange::Attack()
{
}

bool EnemyCreatureRange::IsMoving()
{
    return (mCurrentState == EnemyState::CHASE);
}

void EnemyCreatureRange::Death()
{
    Enemy::Death();
}

void EnemyCreatureRange::Init()
{
    Enemy::Init();
}

void EnemyCreatureRange::RangeAttack()
{
}
