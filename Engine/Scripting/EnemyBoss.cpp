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

#define LASER_WIND_UP 2.625f
#define BULLETS_ANIMATION 146.0f / 24.0f
#define BULLETS_WIND_UP 92.0f / 24.0f
#define ERUPTION_ANIMATION 144.0f / 24.0f
#define IDLE_ANIMATION 40.0f / 24.0f
#define PHASE_ANIMATION 5.0f
#define DEATH_ANIMATION 4.4583f
#define WAKEUP_ANIMATION 2.5f
#define HIT_ANIMATION 1.25f
#define BEAT_TIME 0.428571435f

CREATE(EnemyBoss) {
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::FLOAT, mMaxHealth);
    MEMBER(MemberType::FLOAT, mSpeed);
    MEMBER(MemberType::FLOAT, mRotationSpeed);
    MEMBER(MemberType::FLOAT, mAttackDistance);
    MEMBER(MemberType::FLOAT, mAttackCoolDown);
    MEMBER(MemberType::FLOAT, mAttackSequenceCooldown);
    MEMBER(MemberType::FLOAT, mPhaseShiftTime);
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
    Enemy::Start();
    mCurrentState = EnemyState::DOWN;
    mFront = mGameObject->GetFront();

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
        mAnimationComponent->SetLoop(false);
    }
}

void EnemyBoss::Update()
{
    if (GameManager::GetInstance()->IsPaused()) return;
    if (GameManager::GetInstance()->GetHud()) GameManager::GetInstance()->GetHud()->SetBossHealth(mHealth / mMaxHealth);
    float t = HIT_ANIMATION;
    static short phaseChange = 0;

    if ((mStage == 1 && mHealth / mMaxHealth < mPhase2Hp) || (mStage == 0 && mHealth / mMaxHealth < mPhase1Hp))
    {
        //Phase change
        ++mStage;
        if (mStage == 1) mHealth = mMaxHealth * mPhase1Hp;
        else if (mStage == 2) mHealth = mMaxHealth * mPhase2Hp;
        mCurrentState = EnemyState::PHASE;
        mBulletHell = BulletPattern::NONE;
        if (mAnimationComponent) mAnimationComponent->SendTrigger("tHit1", mDeathTransitionDuration);
        return;
    }

    if (!mBeAttracted)
    {
        switch (mCurrentState)
        {
        case EnemyState::IDLE:
        case EnemyState::ATTACK:

            switch (mStage)
            {
            case 0:
                UpdatePhase1();
                break;
            case 1:
                UpdatePhase2();
                break;
            case 2:
                UpdatePhase3();
                break;
            }
            
            break;
        case EnemyState::PHASE:
            switch (phaseChange)
            {
            case 0:
                if (mPhaseShiftTimer.Delay(HIT_ANIMATION))
                {
                    if (mAnimationComponent) mAnimationComponent->SendTrigger("tDeath", mDeathTransitionDuration);
                    ++phaseChange;
                }
                break;
            case 1:
                if (mPhaseShiftTimer.Delay(DEATH_ANIMATION + mPhaseShiftTime))
                {
                    if (mAnimationComponent) mAnimationComponent->SendTrigger("tWakeUp", 1.0f);
                    ++phaseChange;
                }
                break;
            case 2:
                if (mPhaseShiftTimer.Delay(WAKEUP_ANIMATION))
                {

                    if (mAnimationComponent) mAnimationComponent->SendTrigger("tPhase", mDeathTransitionDuration);
                    ++phaseChange;
                }
                break;
            case 3:
                if (mPhaseShiftTimer.Delay(PHASE_ANIMATION))
                {
                    if (mAnimationComponent) mAnimationComponent->SendTrigger("tIdle", mDeathTransitionDuration);
                    mCurrentState = EnemyState::IDLE;
                    phaseChange = 0;
                }
                break;
            }
           
            break;
        case EnemyState::DEATH:
            if (mAnimationComponent) mAnimationComponent->SendTrigger("tDeath", mDeathTransitionDuration);
            mBulletHell = BulletPattern::NONE;
            Death();
            break;
        case EnemyState::CHARGING_BULLET_HELL:
            if (mPhaseShiftTimer.Delay(BULLETS_WIND_UP))
            {
                if (mAnimationComponent) mAnimationComponent->SendTrigger("tBulletHell", mAttackTransitionDuration);
                mCurrentState = EnemyState::ATTACK;
            }
            break;
        case EnemyState::CHARGING_LASER:
            if (mPhaseShiftTimer.Delay(2.625f))
            {
                mCurrentState = EnemyState::ATTACK;
                if (mAnimationComponent) mAnimationComponent->SendTrigger("tLaser", mAttackTransitionDuration);
            }
            break;
        case EnemyState::WAKE:
            if (mPhaseShiftTimer.Delay(WAKEUP_ANIMATION))
            {
                GameManager::GetInstance()->GetHud()->SetBossHealthBarEnabled(true);
                if (mAnimationComponent) mAnimationComponent->SendTrigger("tIdle", mDeathTransitionDuration);
                mCurrentState = EnemyState::IDLE;
            }
            break;
        case EnemyState::DOWN:
            if (IsPlayerInRange(mBulletRange))
            {
                GameManager::GetInstance()->GetHud()->SetBossHealthBarEnabled(true);
                if (mAnimationComponent) mAnimationComponent->SendTrigger("tWakeUp", mDeathTransitionDuration);
                mCurrentState = EnemyState::WAKE;
            }
        }
    }

    if (mBulletHell != BulletPattern::NONE)
    {
        if (mAttackDurationTimer.Delay(mBulletHellDuration)) mBulletHell = BulletPattern::NONE;
        else switch (mBulletHell)
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
        case BulletPattern::TARGETED_CIRCLES:
            BulletHellPattern6();
            break;
        }
    }

}

void EnemyBoss::StartBulletAttack(BulletPattern pattern)
{
    mCurrentState = EnemyState::CHARGING_LASER;
    if (mAnimationComponent) mAnimationComponent->SendTrigger("tLaserCharge", mAttackTransitionDuration);
    mBulletHell = pattern;
    mBulletsWave = 0;
}

void EnemyBoss::LaserAttack()
{
    mCurrentState = EnemyState::CHARGING_LASER;
    if (mAnimationComponent) mAnimationComponent->SendTrigger("tLaserCharge", mAttackTransitionDuration);
    if (mLaserGO)
    {
        BossLaser* laserScript = static_cast<BossLaser*>(static_cast<ScriptComponent*>(mLaserGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
        if (laserScript) laserScript->Init(mLaserDamage,mLaserDuration,mLaserDistance,mLaserSpeed);
    }
}

void EnemyBoss::BombAttack()
{
    mCurrentState = EnemyState::ATTACK;
    if (mAnimationComponent) mAnimationComponent->SendTrigger("tEruption", mAttackTransitionDuration);
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
        GameManager::GetInstance()->GetHud()->SetBossHealthBarEnabled(false);
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
        for (unsigned int i = 0; i < nBullets; ++i)
        {
            // Give bullet random directon
            float angle = (-pi / 2) + offset + i * alpha;
            GameObject* bulletGO = GameManager::GetInstance()->GetPoolManager()->Spawn(PoolType::ENEMY_BULLET);
            bulletGO->SetWorldPosition(bulletOriginPosition);

            float3 direction = float3(mFront.x * cos(angle) - mFront.z * sin(angle), mFront.y, mFront.x * sin(angle) + mFront.z * cos(angle));
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
        float3 right = mGameObject->GetUp().Cross(direction);
        
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

        for (int i : { -1, 1 })
        {
            // Give bullet random directon
            GameObject* bulletGO = GameManager::GetInstance()->GetPoolManager()->Spawn(PoolType::ENEMY_BULLET);
            float angle = alpha * i;
            float3 direction = float3(mFront.x * cos(angle) - mFront.z * sin(angle), mFront.y, mFront.x * sin(angle) + mFront.z * cos(angle));
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
        
        float3 right = mGameObject->GetUp().Cross(direction);

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

        GameObject* bulletGO = GameManager::GetInstance()->GetPoolManager()->Spawn(PoolType::ENEMY_BULLET);
        float3 direction = float3(mFront.x * cos(alpha) - mFront.z * sin(alpha), mFront.y, mFront.x * sin(alpha) + mFront.z * cos(alpha));
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

void EnemyBoss::UpdatePhase1()
{
    static unsigned int sequence = 0;
    switch (mCurrentState)
    {
    case EnemyState::IDLE:
        if (mAttackCoolDownTimer.Delay(mAttackCoolDown))
        {
            switch (sequence)
            {
            case 0:
                StartBulletAttack(BulletPattern::WAVE);
                break;
            case 1:
                StartBulletAttack(BulletPattern::ARROW);
                break;
            case 2:
                StartBulletAttack(BulletPattern::CIRCLES);
                break;
            }
        }
        break;
    case EnemyState::ATTACK:
        if (mAttackCoolDownTimer.Delay(mBulletHellDuration))
        {
            mCurrentState = EnemyState::IDLE;
            if (mAnimationComponent) mAnimationComponent->SendTrigger("tIdle", mIdleTransitionDuration);
            ++sequence;
            sequence %= 3;
        }
        break;
    }
}

void EnemyBoss::UpdatePhase2()
{
    static unsigned int sequence = 0;
    static unsigned int attack = 0;
    switch (mCurrentState)
    {
    case EnemyState::IDLE:
        if (mAttackCoolDownTimer.Delay(mAttackCoolDown))
        {
            switch (sequence)
            {
            case 0:
                LaserAttack();
                break;
            case 1:
                StartBulletAttack(BulletPattern::WAVE);
                break;
            case 2:
                StartBulletAttack(BulletPattern::TARGETED_CIRCLES);
                break;
            case 3:
                StartBulletAttack(BulletPattern::CIRCLES);
                break;
            }
        }
        break;
    case EnemyState::ATTACK:
        switch (sequence * 10 + attack)
        {
        case 0:
            if (mAttackCoolDownTimer.Delay(mAttackSequenceCooldown))
            {
                StartBulletAttack(BulletPattern::ARROW);
                attack++;
            }
            break;
        case 10:
        case 30:
            if (mAttackCoolDownTimer.Delay(mAttackSequenceCooldown))
            {
                LaserAttack();
                attack++;
            }
            break;
        default:
            if (mAttackCoolDownTimer.Delay(mBulletHellDuration))
            {
                mCurrentState = EnemyState::IDLE;
                if (mAnimationComponent) mAnimationComponent->SendTrigger("tIdle", mIdleTransitionDuration);
                ++sequence;
                sequence %= 4;
                attack = 0;
            }
            break;
        }
    }
}

void EnemyBoss::UpdatePhase3()
{
    static int sequence = -1;
    static unsigned int attack = 0;
    switch (mCurrentState)
    {
    case EnemyState::IDLE:
        if (mAttackCoolDownTimer.Delay(mAttackCoolDown))
        {
            switch (sequence)
            {
            case 0:
                StartBulletAttack(BulletPattern::WAVE);
                break;
            case 1:
                BombAttack();
                break;
            case 2:
                StartBulletAttack(BulletPattern::TARGETED_CIRCLES);
                break;
            case 3:
                StartBulletAttack(BulletPattern::CIRCLES);
                break;
            case -1:// Start with bombs. Never repeat this sequence
                BombAttack();
                break;

            }
        }
        break;
    case EnemyState::ATTACK:
        switch (sequence * 10 + attack)
        {
        case 0:
        case 11:
        case 30:
            if (mAttackCoolDownTimer.Delay(mAttackSequenceCooldown))
            {
                BombAttack();
                attack++;
            }
            break;
        case 1:
        case 10:
        case 20:
            if (mAttackCoolDownTimer.Delay(mAttackSequenceCooldown))
            {
                LaserAttack();
                attack++;
            }
            break;
        case 31:
            if (mAttackCoolDownTimer.Delay(mAttackSequenceCooldown))
            {
                StartBulletAttack(BulletPattern::WAVE);
                attack++;
            }
            break;
        default:
            if (mAttackCoolDownTimer.Delay(mBulletHellDuration))
            {
                mCurrentState = EnemyState::IDLE;
                if (mAnimationComponent) mAnimationComponent->SendTrigger("tIdle", mIdleTransitionDuration);
                ++sequence;
                sequence %= 4;
                attack = 0;
            }
            break;
        }
    }
}

void EnemyBoss::LookAt(float3 target)
{
    mTargetFront = target - mGameObject->GetWorldPosition();
    mTargetFront.Normalize();
}

void EnemyBoss::TakeDamage(float damage)
{
    if (mCurrentState == EnemyState::PHASE) return;
    if (mHealth > 0) // TODO: WITHOUT THIS IF DEATH is called two times
    {
        ActivateHitEffect();
        mHealth -= damage;

        if (mHealth <= 0)
        {
            // Use Hit2 animation before Death??
            mCurrentState = EnemyState::DEATH;
        }
    }
}