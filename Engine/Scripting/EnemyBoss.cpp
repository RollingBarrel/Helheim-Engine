#include "EnemyBoss.h"
#include "GameObject.h"
#include "AIAGentComponent.h"
#include "AnimationComponent.h"
#include "ScriptComponent.h"
#include "GameManager.h"
#include "PoolManager.h"
#include "BombBoss.h"
#include "ColorGradient.h"
#include "Bullet.h"

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
    
    for (const char* prefab : mTemplateNames)
    {
        GameObject* bombTemplate = new GameObject(prefab, mGameObject);
        //bombTemplate->SetEnabled(false);
        mTemplates.push_back(bombTemplate);
    }

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
        switch (rand() % 3)
        {
        case 0:
            BulletAttack();
            break;
        case 1:
            LaserAttack();
            break;
        case 2:
            BombAttack();
            break;
        default:
            break;
        }
    }
}

void EnemyBoss::BulletAttack()
{
    float3 bulletOriginPosition = mGameObject->GetWorldPosition();
    float3 direction = (mPlayer->GetWorldPosition() - bulletOriginPosition);
    direction.y = 0;
    direction.Normalize();
    float angle = std::atan2(direction.x, direction.z);
    GameObject* bulletGO = GameManager::GetInstance()->GetPoolManager()->Spawn(PoolType::ENEMYBULLET);
    bulletGO->SetWorldPosition(bulletOriginPosition);
    bulletGO->SetWorldRotation(float3(0, angle, 0));
    Bullet* bulletScript = reinterpret_cast<Bullet*>(reinterpret_cast<ScriptComponent*>(bulletGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
    ColorGradient gradient;
    gradient.AddColorGradientMark(0.1f, float4(1.0f, 0.0f, 0.0f, 0.0f));
    bulletScript->Init(bulletOriginPosition, mGameObject->GetFront(), mBulletSpeed, 1.0f, &gradient, mRangeDamage);
}

void EnemyBoss::LaserAttack()
{
    
}

void EnemyBoss::BombAttack()
{
    float3 target = mPlayer->GetWorldPosition();
    int index = rand() % mTemplates.size();
    GameObject* bombGO = mTemplates[index];
    bombGO->SetWorldPosition(target);
    std::vector<Component*> scriptComponents;
    bombGO->GetComponentsInChildren(ComponentType::SCRIPT, scriptComponents);
    for (Component* scriptComponent : scriptComponents)
    {
        BombBoss* bombScript = reinterpret_cast<BombBoss*>(reinterpret_cast<ScriptComponent*>(scriptComponent)->GetScriptInstance());
        bombScript->Init(mGameObject->GetWorldPosition());
    }
}

void EnemyBoss::Death()
{
    mGameObject->SetEnabled(false);
}

void EnemyBoss::Reset()
{
    Enemy::Reset();
}