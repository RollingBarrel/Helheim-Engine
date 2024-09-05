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

#define LASER_ANIMATION 78.0f / 20
#define LASER_WIND_UP 38.0f / 20
#define BULLETS_ANIMATION 146.0f / 20
#define BULLETS_WIND_UP 92.0f / 20
#define ERUPTION_ANIMATION 144.0f / 20
#define IDLE_ANIMATION 40.0f / 20
#define PHASE_ANIMATION 120.0f / 20
#define DEATH_ANIMATION 107.0f / 20
#define BEAT_TIME 0.428571435f

CREATE(EnemyBoss) {
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::FLOAT, mMaxHealth);
    MEMBER(MemberType::FLOAT, mSpeed);
    MEMBER(MemberType::FLOAT, mRotationSpeed);
    MEMBER(MemberType::FLOAT, mAttackDistance);
    MEMBER(MemberType::FLOAT, mAttackCoolDown);
    MEMBER(MemberType::FLOAT, mPhase1Hp);
    MEMBER(MemberType::FLOAT, mPhase2Hp);
    MEMBER(MemberType::FLOAT, mDeathTime);
    SEPARATOR("BULLET HELL");
    MEMBER(MemberType::FLOAT, mBulletHellDuration);
    MEMBER(MemberType::FLOAT, mBulletSpeed);
    MEMBER(MemberType::FLOAT, mBulletRange);
    MEMBER(MemberType::FLOAT, mBulletsDamage);
    SEPARATOR("BOMBS");
    MEMBER(MemberType::FLOAT, mBombsDuration);
    MEMBER(MemberType::FLOAT, mBombsDelay);
    MEMBER(MemberType::FLOAT, mBombDamage);
    SEPARATOR("LASER");
    MEMBER(MemberType::GAMEOBJECT, mLaserGO);
    MEMBER(MemberType::FLOAT, mLaserDuration);
    MEMBER(MemberType::FLOAT, mLaserDamage);
    MEMBER(MemberType::FLOAT, mLaserDistance);
    MEMBER(MemberType::FLOAT, mLaserSpeed);

    END_CREATE;
}

EnemyBoss::EnemyBoss(GameObject* owner) : Enemy(owner)
{
}

void EnemyBoss::Start()
{
    srand(static_cast<unsigned int>(time(0)));

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
            if ((mStage == 1 && mHealth / mMaxHealth < mPhase2Hp) || (mStage == 0 && mHealth / mMaxHealth < mPhase1Hp))
            {
                //Phase change
                ++mStage;
                mCurrentState = EnemyState::PHASE;
                if (mAnimationComponent) mAnimationComponent->SendTrigger("tPhase", mDeathTransitionDuration);
            }
            else if (mAttackCoolDownTimer.Delay(mAttackCoolDown) && IsPlayerInRange(mBulletRange))
            {
                GameManager::GetInstance()->GetHud()->SetBossHealthBarEnabled(true);
                mCurrentState = EnemyState::ATTACK;
                SelectAttack();
            }
            break;
        case EnemyState::ATTACK:
            if (mAttackDurationTimer.Delay(mAttackDuration))
            {
                mBulletHell = BulletPattern::NONE;
                if (mAnimationComponent) mAnimationComponent->SendTrigger("tIdle", mIdleTransitionDuration);
                mCurrentState = EnemyState::IDLE;
            }
            else if (mBulletHell != BulletPattern::NONE)
            {
                switch (mBulletHell)
                {
                case BulletPattern::CIRCLES:
                    BulletHellPattern1();
                    break;
                case BulletPattern::ARROW:
                    BulletHellPattern2();
                    break;
                case BulletPattern::WAVE:
                    BulletHellPattern5();
                    break;
                }
            }
            break;
        case EnemyState::PHASE:
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
    if (attack == mLastAttack)
    {
        ++attack;
        attack %= 3;
    }
    mLastAttack = attack;

    attack += mStage * 10;
    switch (attack)
    {
    case 1:
        if (mAnimationComponent) mAnimationComponent->SendTrigger("tLaser", mAttackTransitionDuration);
        //if (mAnimationComponent) mAnimationComponent->SetAnimSpeed(LASER_ANIMATION / mAttackDuration);
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
    mBulletHell = static_cast<BulletPattern>(rand() % BulletPattern::NONE);
    mBulletsWave = 0;
}

void EnemyBoss::LaserAttack()
{
    if (mLaserGO)
    {
        BossLaser* laserScript = static_cast<BossLaser*>(static_cast<ScriptComponent*>(mLaserGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
        if (laserScript) laserScript->Init(mLaserDamage,mLaserDuration,mLaserDistance,mLaserSpeed);
    }
}

void EnemyBoss::BombAttack()
{
    float3 target = mPlayer->GetWorldPosition();
    int index = rand() % mTemplates.size();
    GameObject* bombGO = mTemplates[index];
	//LOG("Bomb index: %d", index);
    bombGO->SetWorldPosition(target);
	float randRotation = static_cast<float>(rand() % 180);
	float3 bombRotation = float3(0.0f, randRotation, 0.0f);
	bombGO->SetWorldRotation(bombRotation);
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

void EnemyBoss::BulletHellPattern1() //Circular
{
    if (mBulletHellTimer.Delay(4 * BEAT_TIME)) //Each pattern will need different rythm
    {
        unsigned int nBullets = 10;
        float alpha = DegToRad(180 / (nBullets - 1));
        float offset = 0.0f;
        if (mBulletsWave % 2 == 1)
        {
            offset = alpha / 2;
            nBullets--;
        }
        float3 bulletOriginPosition = mGameObject->GetWorldPosition();
        bulletOriginPosition.y = mPlayer->GetWorldPosition().y + 2.0f;
        float3 rotation = mGameObject->GetWorldEulerAngles();
        float3 front = mGameObject->GetFront();
        for (unsigned int i = 0; i < nBullets; ++i)
        {
            // Give bullet random directon
            float angle = (-pi / 2) + offset + i * alpha;
            GameObject* bulletGO = GameManager::GetInstance()->GetPoolManager()->Spawn(PoolType::ENEMY_BULLET);
            bulletGO->SetWorldPosition(bulletOriginPosition);

            float3 direction = float3(front.x * cos(angle) - front.z * sin(angle), front.y, front.x * sin(angle) + front.z * cos(angle));
            direction.Normalize();
            Bullet* bulletScript = static_cast<Bullet*>(static_cast<ScriptComponent*>(bulletGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
            ColorGradient gradient;
            gradient.AddColorGradientMark(0.1f, float4(255.0f, 255.0f, 255.0f, 1.0f));
            bulletScript->Init(bulletOriginPosition, direction, mBulletSpeed, 1.0f, &gradient, mBulletsDamage, mBulletRange);
        }
        mBulletsWave++;
    }
}

void EnemyBoss::BulletHellPattern2() //Arrow
{
    
    const unsigned int nBullets = 8;
    float delay = 0.25f * BEAT_TIME;
    if (mBulletsWave % nBullets == 0) delay = 2.0f * BEAT_TIME;
    if (mBulletHellTimer.Delay(delay)) //Each pattern will need different rythm
    {
        const float space = 0.3f;
        
        float3 bulletOriginPosition = mGameObject->GetWorldPosition();
        float3 target = mPlayer->GetWorldPosition();
        target.y += 2.0f;
        bulletOriginPosition.y = target.y;
        static float3 direction = (target - bulletOriginPosition).Normalized();
        if (mBulletsWave % nBullets == 0)
        {
            direction = target - bulletOriginPosition;
            direction.Normalize();
        }
        float3 front = mGameObject->GetFront();
        float3 right = mGameObject->GetRight();
        
        for (int i : { -1, 1 })
        {
            // Give bullet random directon
            GameObject* bulletGO = GameManager::GetInstance()->GetPoolManager()->Spawn(PoolType::ENEMY_BULLET);
            float3 position = bulletOriginPosition + right * space * (mBulletsWave % nBullets) * i;

            Bullet* bulletScript = static_cast<Bullet*>(static_cast<ScriptComponent*>(bulletGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
            ColorGradient gradient;
            gradient.AddColorGradientMark(0.1f, float4(255.0f, 255.0f, 255.0f, 1.0f));
            bulletScript->Init(position, direction, mBulletSpeed, 1.0f, &gradient, mBulletsDamage, mBulletRange);
        }
        mBulletsWave++;
    }
}

void EnemyBoss::BulletHellPattern3() //Two streams
{
    
    if (mBulletHellTimer.Delay(0.25f * BEAT_TIME)) //Each pattern will need different rythm
    {
        const unsigned int nBullets = 16;
        float alpha = (pi / 2) - pi * (mBulletsWave % nBullets) / (nBullets - 1);
        float3 bulletOriginPosition = mGameObject->GetWorldPosition();
        bulletOriginPosition.y = mPlayer->GetWorldPosition().y + 2.0f;
        float3 front = mGameObject->GetFront();

        for (int i : { -1, 1 })
        {
            // Give bullet random directon
            GameObject* bulletGO = GameManager::GetInstance()->GetPoolManager()->Spawn(PoolType::ENEMY_BULLET);
            float angle = alpha * i;
            float3 direction = float3(front.x * cos(angle) - front.z * sin(angle), front.y, front.x * sin(angle) + front.z * cos(angle));
            direction.Normalize();
            Bullet* bulletScript = static_cast<Bullet*>(static_cast<ScriptComponent*>(bulletGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
            ColorGradient gradient;
            gradient.AddColorGradientMark(0.1f, float4(255.0f, 255.0f, 255.0f, 1.0f));
            bulletScript->Init(bulletOriginPosition, direction, mBulletSpeed, 1.0f, &gradient, mBulletsDamage, mBulletRange);
        }
        mBulletsWave++;
    }
}

void EnemyBoss::BulletHellPattern4() //Curved Arrows
{
    
    const unsigned int nBullets = 8;
    const float width = 2.0f;
    float delay = 0.25f * BEAT_TIME;
    if (mBulletsWave % nBullets == 0) delay = 2.0f * BEAT_TIME;
    if (mBulletHellTimer.Delay(delay)) //Each pattern will need different rythm
    {
        
        float3 bulletOriginPosition = mGameObject->GetWorldPosition();
        float3 target = mPlayer->GetWorldPosition();
        target.y += 2.0f;
        bulletOriginPosition.y = target.y;
        static float3 direction = (target - bulletOriginPosition).Normalized();
        if (mBulletsWave % nBullets == 0)
        {
            direction = target - bulletOriginPosition;
            direction.Normalize();
        }
        
        float3 front = mGameObject->GetFront();
        float3 right = mGameObject->GetRight();

        for (int i : { -1, 1 })
        {
            // Give bullet random directon
            GameObject* bulletGO = GameManager::GetInstance()->GetPoolManager()->Spawn(PoolType::ENEMY_BULLET);
            float3 position = bulletOriginPosition + right * width * sin((pi*3/4)* (mBulletsWave % nBullets) / (nBullets - 1)) * i;

            Bullet* bulletScript = static_cast<Bullet*>(static_cast<ScriptComponent*>(bulletGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
            ColorGradient gradient;
            gradient.AddColorGradientMark(0.1f, float4(255.0f, 255.0f, 255.0f, 1.0f));
            bulletScript->Init(position, direction, mBulletSpeed, 1.0f, &gradient, mBulletsDamage, mBulletRange);
        }
        mBulletsWave++;
    }
}

void EnemyBoss::BulletHellPattern5() //Stream
{

    if (mBulletHellTimer.Delay(BEAT_TIME/8)) //Each pattern will need different rythm
    {
        const unsigned int nBullets = 16;
        float alpha = (pi / 2) - pi * (mBulletsWave % nBullets) / (nBullets - 1);
        if (mBulletsWave % (2*nBullets) >= nBullets)
        {
            alpha = pi / (nBullets * 2) - alpha;
        }
        float3 bulletOriginPosition = mGameObject->GetWorldPosition();
        bulletOriginPosition.y = mPlayer->GetWorldPosition().y + 2.0f;
        float3 front = mGameObject->GetFront();

        GameObject* bulletGO = GameManager::GetInstance()->GetPoolManager()->Spawn(PoolType::ENEMY_BULLET);
        float3 direction = float3(front.x * cos(alpha) - front.z * sin(alpha), front.y, front.x * sin(alpha) + front.z * cos(alpha));
        direction.Normalize();
        Bullet* bulletScript = static_cast<Bullet*>(static_cast<ScriptComponent*>(bulletGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
        ColorGradient gradient;
        gradient.AddColorGradientMark(0.1f, float4(255.0f, 255.0f, 255.0f, 1.0f));
        bulletScript->Init(bulletOriginPosition, direction, mBulletSpeed, 1.0f, &gradient, mBulletsDamage, mBulletRange);
        mBulletsWave++;
    }
}

void EnemyBoss::BulletHellPattern6() //Aimed circles
{
    const float radius = 10.0f;
    if (mBulletHellTimer.Delay(BEAT_TIME * 4)) //Each pattern will need different rythm
    {
        const unsigned int nBullets = 16;
        float3 target = mPlayer->GetWorldPosition();
        float3 front = mPlayer->GetFront();
        for (int i = 0; i < nBullets; i++)
        {
            float alpha = 2 * pi * i / nBullets;
            float3 direction = float3(front.x * cos(alpha) - front.z * sin(alpha), front.y, front.x * sin(alpha) + front.z * cos(alpha));
            direction.Normalize();
            GameObject* bulletGO = GameManager::GetInstance()->GetPoolManager()->Spawn(PoolType::ENEMY_BULLET);
            Bullet* bulletScript = static_cast<Bullet*>(static_cast<ScriptComponent*>(bulletGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
            ColorGradient gradient;
            gradient.AddColorGradientMark(0.1f, float4(255.0f, 255.0f, 255.0f, 1.0f));
            bulletScript->Init(target - direction*radius, direction, mBulletSpeed, 1.0f, &gradient, mBulletsDamage, mBulletRange);
        }
    }
}