#include "EnemyBoss.h"
#include "Application.h"
#include "GameObject.h"
#include "ModuleScene.h"
#include "AIAGentComponent.h"
#include "AnimationComponent.h"
#include "ScriptComponent.h"
#include "GameManager.h"
#include "PoolManager.h"
#include "BombBoss.h"
#include "ColorGradient.h"
#include "Bullet.h"
#include "BossLaser.h"

CREATE(EnemyBoss) {
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::FLOAT, mMaxHealth);
    MEMBER(MemberType::FLOAT, mSpeed);
    MEMBER(MemberType::FLOAT, mRotationSpeed);
    MEMBER(MemberType::FLOAT, mAttackDistance);
    MEMBER(MemberType::FLOAT, mAttackDamage);
    MEMBER(MemberType::FLOAT, mBulletSpeed);
    MEMBER(MemberType::FLOAT, mAttackCoolDown);
    MEMBER(MemberType::GAMEOBJECT, mLaserGO);

    END_CREATE;
}

EnemyBoss::EnemyBoss(GameObject* owner) : Enemy(owner)
{
}

void EnemyBoss::Start()
{
    Enemy::Start();

    
    for (const char* prefab : mTemplateNames)
    {
        GameObject* bombTemplate = App->GetScene()->InstantiatePrefab(prefab, mGameObject);
        if (bombTemplate)
        {
            bombTemplate->SetEnabled(false);
            mTemplates.push_back(bombTemplate);
        }
    }

    mAnimationComponent = reinterpret_cast<AnimationComponent*>(mGameObject->GetComponent(ComponentType::ANIMATION));
    if (mAnimationComponent)
    {
        mAnimationComponent->SetIsPlaying(true);

    }
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
    if (IsPlayerInRange(50))
    {
        mAnimationComponent->SendTrigger("tWakeUp", 0.2f);
        mCurrentState = BossState::ATTACK;
    }
}

void EnemyBoss::Attack()
{
    if (mAttackDurationTimer.Delay(mAttackCoolDown))
    {
        int attack = rand() % 3;
        if (attack == mLastAttack)
        {
            ++attack;
        }
        switch (attack)
        {
        case 1:
            mAnimationComponent->SendTrigger("tLaser", 0.2f);
            LaserAttack();
            break;
        case 2:
            mAnimationComponent->SendTrigger("tEruption", 0.2f);
            BombAttack();
            break;
        case 0:
        default:
            mAnimationComponent->SendTrigger("tBulletHell", 0.2f);
            BulletAttack();
            break;
        }
    }
}

void EnemyBoss::BulletAttack()
{
    float3 bulletOriginPosition = mGameObject->GetWorldPosition();
    GameObject* bulletGO = GameManager::GetInstance()->GetPoolManager()->Spawn(PoolType::ENEMY_BULLET);
    bulletGO->SetWorldPosition(bulletOriginPosition);
    bulletGO->LookAt(mPlayer->GetWorldPosition());
    Bullet* bulletScript = reinterpret_cast<Bullet*>(reinterpret_cast<ScriptComponent*>(bulletGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
    ColorGradient gradient;
    gradient.AddColorGradientMark(0.1f, float4(1.0f, 0.0f, 0.0f, 0.0f));
    bulletScript->Init(bulletOriginPosition, bulletGO->GetFront(), mBulletSpeed, 1.0f, &gradient, mAttackDamage);
}

void EnemyBoss::LaserAttack()
{
    if (mLaserGO)
    {
        BossLaser* laserScript = reinterpret_cast<BossLaser*>(reinterpret_cast<ScriptComponent*>(mLaserGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
        if (laserScript) laserScript->Init();
    }
    
}

void EnemyBoss::BombAttack()
{
    float3 target = mPlayer->GetWorldPosition();
    int index = rand() % mTemplates.size();
    GameObject* bombGO = mTemplates[index];
    bombGO->SetWorldPosition(target);
    std::vector<Component*> scriptComponents;
    bombGO->GetComponentsInChildren(ComponentType::SCRIPT, scriptComponents);
    bombGO->SetEnabled(true);
    for (Component* scriptComponent : scriptComponents)
    {
        BombBoss* bombScript = reinterpret_cast<BombBoss*>(reinterpret_cast<ScriptComponent*>(scriptComponent)->GetScriptInstance());
        bombScript->Init(mGameObject->GetWorldPosition());
    }
}

void EnemyBoss::Death()
{
    mAnimationComponent->SendTrigger("tDeath", 0.2f);
    mDeathTimer.Delay(mDeathTime);
    mGameObject->SetEnabled(false);
}