#include "EnemyBoss.h"

CREATE(EnemyBoss) {
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