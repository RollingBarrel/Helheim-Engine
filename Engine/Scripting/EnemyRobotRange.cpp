#include "EnemyRobotRange.h"
#include "GameObject.h"
#include "AnimationComponent.h"
#include "ScriptComponent.h"
#include "AIAGentComponent.h"

#include "GameManager.h"
#include "PoolManager.h"
#include "Bullet.h"

#include "ColorGradient.h"

CREATE(EnemyRobotRange) 
{
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::FLOAT, mMaxHealth);
    MEMBER(MemberType::FLOAT, mSpeed);
    MEMBER(MemberType::FLOAT, mRotationSpeed);
    MEMBER(MemberType::FLOAT, mRangeDamage);
    MEMBER(MemberType::FLOAT, mBulletSpeed);
    MEMBER(MemberType::GAMEOBJECT, mBulletOrigin);
    SEPARATOR("STATES");
    MEMBER(MemberType::FLOAT, mAttackDistance);

    END_CREATE;
}

void EnemyRobotRange::Attack()
{
    Enemy::Attack();

    float3 direction = (mPlayer->GetWorldPosition() - mGameObject->GetWorldPosition());
    direction.y = 0;
    direction.Normalize();
    float angle = std::atan2(direction.x, direction.z);

    if (mGameObject->GetWorldRotation().y != angle)
    {
        mGameObject->SetWorldRotation(float3(0, angle, 0));

    }
    if (mAttackCoolDownTimer.Delay(mAttackCoolDown)) 
    {
        mAnimationComponent->SendTrigger("tAttack", 0.2f);
        RangeAttack();

    }
}

void EnemyRobotRange::RangeAttack()
{
    float3 bulletOriginPosition = mBulletOrigin->GetWorldPosition();
    GameObject* bulletGO = GameManager::GetInstance()->GetPoolManager()->Spawn(PoolType::ENEMY_BULLET);
    bulletGO->SetWorldPosition(bulletOriginPosition);
    bulletGO->SetWorldRotation(mGameObject->GetWorldRotation());
    Bullet* bulletScript=reinterpret_cast<Bullet*>(reinterpret_cast<ScriptComponent*>(bulletGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
    ColorGradient gradient;
    gradient.AddColorGradientMark(0.1f, float4(255.0f, 255.0f, 255.0f, 1.0f));
    bulletScript->Init(bulletOriginPosition, mGameObject->GetFront(),mBulletSpeed,1.0f, &gradient,mRangeDamage);
}
