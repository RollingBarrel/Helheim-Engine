#include "BossLaser.h"
#include "Application.h"
#include "GameObject.h"
#include "ModuleScene.h"
#include "ScriptComponent.h"
#include "GameManager.h"

#include "MathFunc.h"
#include "BossLaserEyeBall.h"

CREATE(BossLaser)
{
    CLASS(owner);
    MEMBER(MemberType::FLOAT, mBossDistance);
    MEMBER(MemberType::INT, mPoolSize);
    END_CREATE;
}

BossLaser::BossLaser(GameObject* owner) : Script(owner)
{
}

void BossLaser::Start()
{
    mCurrentState = LaserState::IDLE;

    for (size_t i = 0; i < mPoolSize; ++i)
    {
        GameObject* eyeBall = App->GetScene()->InstantiatePrefab("BossLaser_EyeBall.prfb",mGameObject);
        if (eyeBall)
        {
            eyeBall->SetEnabled(false);
            eyeBall->SetLocalScale(float3::one * 0.5f);
            mInactiveEyeBall.push_back(eyeBall);
        }
    }
}

void BossLaser::Update()
{
    if (GameManager::GetInstance()->IsPaused()) return;

    float deltaTime = App->GetDt();
    mStateTime += deltaTime;

    switch (mCurrentState)
    {
    case LaserState::IDLE:
        break;
    case LaserState::CHARGING:
        if (mStateTime >= mChargeTime)
        {
            Fire();
        }
        break;
    case LaserState::FIRING:
        if (mStateTime >= mLaserEnemyDuration)
        {
            Cooldown();
        }
        break;
    case LaserState::COOLDOWN:
        if (mStateTime >= mCooldownDuration)
        {
            mCurrentState = LaserState::IDLE;
        }
        break;
    }
}

void BossLaser::Init(float damage, float duration, float distance,float speed)
{
    mDamage = damage;
    mLaserEnemyDuration = duration;
    mLaserDistance = distance;
    mLaserSpeed = speed;    
    Charge();
}

void BossLaser::Charge()
{
    mCurrentState = LaserState::CHARGING;
    mStateTime = 0.0f;
}

void BossLaser::Fire()
{
    mCurrentState = LaserState::FIRING;
    mStateTime = 0.0f;
    SpawnEyeBalls();
}

void BossLaser::Cooldown()
{
    mCurrentState = LaserState::COOLDOWN;
    mStateTime = 0.0f;

    for (GameObject* eyeBall : mActiveEyeBalls)
    {
        eyeBall->SetEnabled(false);
    }
    mActiveEyeBalls.clear();

    ReturnEyeBallsToPool();
}

void BossLaser::SpawnEyeBalls()
{
    float totalArc = 180.0f;
    float angleStep = totalArc / (mPoolSize - 1);

    float3 bossPosition = mGameObject->GetWorldPosition();
    float3 bossFront = mGameObject->GetFront();
    float3 bossRight = mGameObject->GetRight();

    for (int i = 0; i < mPoolSize; ++i)
    {
        if (i >= mInactiveEyeBall.size())
            break;

        GameObject* eyeBall = mInactiveEyeBall[i];

        float angle = DegToRad(-90.0f + i * angleStep); 
        float3 positionOffset = bossFront * std::cos(angle) * mBossDistance + bossRight * std::sin(angle) * mBossDistance;
        positionOffset.y = -1.0f;

        eyeBall->SetWorldPosition(bossPosition + positionOffset);
        eyeBall->SetEnabled(true);

        BossLaserEyeBall* eyeBallScript = static_cast<BossLaserEyeBall*>(static_cast<ScriptComponent*>(eyeBall->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
        if (eyeBallScript)
        {
            eyeBallScript->Init(mDamage, mLaserEnemyDuration, mLaserDistance, mLaserSpeed, angle);
        }

        mActiveEyeBalls.push_back(eyeBall);
    }
}


void BossLaser::ReturnEyeBallsToPool()
{
    for (GameObject* eyeBall : mActiveEyeBalls)
    {
        eyeBall->SetEnabled(false);
        mInactiveEyeBall.push_back(eyeBall);  
    }
    mActiveEyeBalls.clear();
}
