#include "EnemyBoss.h"
#include "GameObject.h"
#include "AIAGentComponent.h"
#include "AnimationComponent.h"
#include "ScriptComponent.h"
#include "GameManager.h"
#include "PoolManager.h"
#include "BombBoss.h"

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

EnemyBoss::EnemyBoss(GameObject* owner) : Enemy(owner)
{
}

void EnemyBoss::Start()
{
    Enemy::Start();

    //mAiAgentComponent = reinterpret_cast<AIAgentComponent*>(mGameObject->GetComponent(ComponentType::AIAGENT));

    mAnimationComponent = reinterpret_cast<AnimationComponent*>(mGameObject->GetComponent(ComponentType::ANIMATION));
    if (mAnimationComponent)
    {
        mAnimationComponent->SetIsPlaying(true);

    }
    mAttackCD = mTimerAttack;
}

void EnemyBoss::Update()
{
    if (GameManager::GetInstance()->IsPaused()) return;

    Enemy::Update();

    if (!mBeAttracted)
    {
        switch (mCurrentState)
        {
        case BossState::IDLE:
            //mAnimationComponent->SendTrigger("tIdle", 0.2f);
            Idle();

            break;
        case BossState::ATTACK:

            Attack();
            break;
        }
    }

}

void EnemyBoss::Idle()
{
    if (IsPlayerInRange(mActivationRange))
    {
        mCurrentState = BossState::ATTACK;
    }
}

void EnemyBoss::Attack()
{
    if (Delay(mTimerAttack))
    {
        BombAttack();
    }
}

void EnemyBoss::BulletAttack()
{

}

void EnemyBoss::LaserAttack()
{

}

void EnemyBoss::BombAttack()
{
    float3 target = mPlayer->GetWorldPosition();
    GameObject* bombGO = GameManager::GetInstance()->GetPoolManager()->Spawn(PoolType::ENEMYBOMB);
    bombGO->SetWorldPosition(target);
    BombBoss* bombScript = reinterpret_cast<BombBoss*>(reinterpret_cast<ScriptComponent*>(bombGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
    bombScript->Init();
}

void EnemyBoss::Death()
{
    mGameObject->SetEnabled(false);
}

void EnemyBoss::Reset()
{

}