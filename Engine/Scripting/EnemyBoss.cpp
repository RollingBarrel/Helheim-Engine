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
#include "HudController.h"
#include "MathFunc.h"

CREATE(EnemyBoss) {
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::FLOAT, mMaxHealth);
    MEMBER(MemberType::FLOAT, mSpeed);
    MEMBER(MemberType::FLOAT, mRotationSpeed);
    MEMBER(MemberType::FLOAT, mAttackDistance);
    MEMBER(MemberType::FLOAT, mAttackCooldown);
    MEMBER(MemberType::FLOAT, mDeathTime);
    SEPARATOR("BULLET HELL");
    MEMBER(MemberType::FLOAT, mBulletHellDuration);
    MEMBER(MemberType::FLOAT, mBulletHellCooldown);
    MEMBER(MemberType::FLOAT, mBulletSpeed);
    MEMBER(MemberType::FLOAT, mBulletsDamage);    
    MEMBER(MemberType::FLOAT, mBulletHellAngleSpread);
    SEPARATOR("BOMBS");
    MEMBER(MemberType::FLOAT, mBombsDuration);
    MEMBER(MemberType::FLOAT, mBombDamage);
    SEPARATOR("LASER");
    MEMBER(MemberType::FLOAT, mLaserDuration);
    MEMBER(MemberType::FLOAT, mLaserDamage);
    MEMBER(MemberType::GAMEOBJECT, mLaserGO);

    END_CREATE;
}

EnemyBoss::EnemyBoss(GameObject* owner) : Enemy(owner)
{
}

void EnemyBoss::Start()
{
    Enemy::Start();
    mCurrentState = EnemyState::IDLE;

    for (const char* prefab : mTemplateNames)
    {
        GameObject* bombTemplate = App->GetScene()->InstantiatePrefab(prefab, mGameObject);
        if (bombTemplate)
        {
            bombTemplate->SetEnabled(false);
            mTemplates.push_back(bombTemplate);
        }
    }

    mAnimationComponent = static_cast<AnimationComponent*>(mGameObject->GetComponent(ComponentType::ANIMATION));
    if (mAnimationComponent)
    {
        mAnimationComponent->SetIsPlaying(true);
        mAnimationComponent->SendTrigger("tIdle", mIdleTransitionDuration);
    }
}

void EnemyBoss::Update()
{
    if (GameManager::GetInstance()->IsPaused()) return;
    if (GameManager::GetInstance()->GetHud()) GameManager::GetInstance()->GetHud()->SetBossHealth(mHealth / mMaxHealth);

    

    if (!mBeAttracted)
    {
        switch (mCurrentState)
        {
        case EnemyState::IDLE:
            if ((mStage == 1 && mHealth / mMaxHealth < 0.2) || (mStage == 0 && mHealth / mMaxHealth < 0.6))
            {
                //Phase change
                ++mStage;
                mCurrentState = EnemyState::CHARGE;
                if (mAnimationComponent) mAnimationComponent->SendTrigger("tPhase", mDeathTransitionDuration);
            }
            else if (mAttackCoolDownTimer.Delay(mAttackCoolDown) && IsPlayerInRange(50))
            {
                GameManager::GetInstance()->GetHud()->SetBossHealthBarEnabled(true);
                mCurrentState = EnemyState::ATTACK;
                SelectAttack();
            }
            break;
        case EnemyState::ATTACK:
            if (mAttackDurationTimer.Delay(mAttackDuration))
            {
                mBulletHell = false;
                if (mAnimationComponent) mAnimationComponent->SendTrigger("tIdle", mIdleTransitionDuration);
                mCurrentState = EnemyState::IDLE;
            }
            else if (mBulletHell && mBulletHellTimer.Delay(mBulletHellCooldown))
            {
                BulletAttack();
            }
            break;
        case EnemyState::CHARGE:
            if (mPhaseShiftTimer.Delay(mPhaseShiftTime))
            {
                mCurrentState = EnemyState::ATTACK;
                SelectAttack();
            }
            break;
        case EnemyState::DEATH:
            if (mAnimationComponent) mAnimationComponent->SendTrigger("tDeath", mDeathTransitionDuration);
            Death();
            break;
        }
    }

}

void EnemyBoss::SelectAttack()
{
    short attack = rand() % 3;
    /*if (attack == mLastAttack)
    {
        ++attack;
        attack %= 3;
    }
    mLastAttack = attack;*/

    attack += mStage * 10;
    switch (attack)
    {
    case 1:
        if (mAnimationComponent) mAnimationComponent->SendTrigger("tLaser", mAttackTransitionDuration);
        LaserAttack();
        mAttackDuration = mLaserDuration;
        break;
    case 2:
        if (mAnimationComponent) mAnimationComponent->SendTrigger("tEruption", mAttackTransitionDuration);
        BombAttack();
        mAttackDuration = mBombsDuration;
        break;
    case 0:
        if (mAnimationComponent) mAnimationComponent->SendTrigger("tBulletHell", mAttackTransitionDuration);
        StartBulletAttack();
        mAttackDuration = mBulletHellDuration;
        break;
    case 10:
        if (mAnimationComponent) mAnimationComponent->SendTrigger("tLaser", mAttackTransitionDuration);
        LaserAttack();
        StartBulletAttack();
        mAttackDuration = Max(mLaserDuration, mBulletHellDuration);
        break;
    case 11:
        if (mAnimationComponent) mAnimationComponent->SendTrigger("tBulletHell", mAttackTransitionDuration);
        StartBulletAttack();
        BombAttack();
        mAttackDuration = Max(mBombsDuration, mBulletHellDuration);
        break;
    case 12:
        if (mAnimationComponent) mAnimationComponent->SendTrigger("tEruption", mAttackTransitionDuration);
        LaserAttack();
        BombAttack();
        mAttackDuration = Max(mLaserDuration, mBombsDuration);
        break;
    case 20:
    case 21:
    case 22:
        if (mAnimationComponent) mAnimationComponent->SendTrigger("tEruption", mAttackTransitionDuration);
        LaserAttack();
        BombAttack();
        StartBulletAttack();
        mAttackDuration = Max(mLaserDuration, mBulletHellDuration, mBombsDuration);
        break;
    }
    
}

void EnemyBoss::StartBulletAttack()
{
    mBulletHell = true;
}

void EnemyBoss::BulletAttack()
{
    auto randFloat = []() -> float
        {
            return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        };

    float3 bulletOriginPosition = mGameObject->GetWorldPosition();
    bulletOriginPosition.y = mPlayer->GetWorldPosition().y + 2.0f;
    float3 rotation = mGameObject->GetWorldEulerAngles();

    // Give bullet random directon
    float angle = randFloat() * mBulletHellAngleSpread - mBulletHellAngleSpread / 2;
    angle = DegToRad(angle);
    GameObject* bulletGO = GameManager::GetInstance()->GetPoolManager()->Spawn(PoolType::ENEMY_BULLET);
    bulletGO->SetWorldPosition(bulletOriginPosition);
    //bulletGO->SetWorldRotation(rotation);
    
    float3 front = mGameObject->GetFront();
    float3 direction = float3(front.x * cos(angle) - front.z * sin(angle), front.y, front.x * sin(angle) + front.z * cos(angle));
    Bullet* bulletScript = static_cast<Bullet*>(static_cast<ScriptComponent*>(bulletGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
    ColorGradient gradient;
    gradient.AddColorGradientMark(0.1f, float4(255.0f, 255.0f, 255.0f, 1.0f));
    bulletScript->Init(bulletOriginPosition, direction, mBulletSpeed, 1.0f, &gradient, mBulletsDamage);
}

void EnemyBoss::LaserAttack()
{
    if (mLaserGO)
    {
        BossLaser* laserScript = static_cast<BossLaser*>(static_cast<ScriptComponent*>(mLaserGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
        if (laserScript) laserScript->Init(mLaserDamage, mAttackDistance);
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
        BombBoss* bombScript = static_cast<BombBoss*>(static_cast<ScriptComponent*>(scriptComponent)->GetScriptInstance());
        bombScript->Init(mGameObject->GetWorldPosition(), mBombDamage);
    }
}

void EnemyBoss::Death()
{
    if (mDeathTimer.Delay(mDeathTime))
    {
        mGameObject->SetEnabled(false);
        GameManager::GetInstance()->Victory();
    }

}