#include "BossLaser.h"
#include "Application.h"
#include "GameObject.h"
#include "BoxColliderComponent.h"
#include "ModuleScene.h"
#include "ScriptComponent.h"

#include "MathFunc.h"
#include "BossLaserEyeBall.h"

CREATE(BossLaser)
{
    CLASS(owner);
    MEMBER(MemberType::FLOAT, mDistance);
    MEMBER(MemberType::FLOAT, mCooldownDuration);
    MEMBER(MemberType::FLOAT, mChargeTime);
    MEMBER(MemberType::FLOAT, mLaserEnemyDuration);
    MEMBER(MemberType::FLOAT, mEyeRotationSpeed);
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
        GameObject* eyeBall = App->GetScene()->InstantiatePrefab("BossLaser_EyeBall.prfb", nullptr);
        if (eyeBall)
        {
            eyeBall->SetEnabled(false);
            mEyeBallPool.push_back(eyeBall);
        }
    }
}

void BossLaser::Update()
{
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

void BossLaser::Init(float damage, float distance)
{
    mDistance = distance;
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

    for (GameObject* eyeBall : mEyeBalls)
    {
        eyeBall->SetEnabled(false);
    }
    mEyeBalls.clear();

    ReturnEyeBallsToPool();
}

void BossLaser::SpawnEyeBalls()
{
    float baseAngle = -90.0f; // Starting angle for the first eye ball
    float angleIncrement = 180.0f / 5; // Divide the semi-circle into 5 parts

    for (size_t i = 0; i < mPoolSize; ++i)
    {
        if (i >= mEyeBallPool.size())
            break;

        GameObject* eyeBall = mEyeBallPool[i];
        float initialRotation = baseAngle + i * angleIncrement;

        float radians = DegToRad(initialRotation);
        float x = std::cos(radians) * mDistance;
        float z = std::sin(radians) * mDistance;
        float3 positionOffset = float3(x, -2, z);

        eyeBall->SetWorldPosition(mGameObject->GetWorldPosition() + positionOffset);
        eyeBall->SetEnabled(true);

        BossLaserEyeBall* eyeBallScript = static_cast<BossLaserEyeBall*>(static_cast<ScriptComponent*>(eyeBall->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
        if (eyeBallScript)
        {
            eyeBallScript->Init(mDistance, mLaserEnemyDuration, angleIncrement);
        }

        mEyeBalls.push_back(eyeBall);
    }
}


void BossLaser::ReturnEyeBallsToPool()
{
    for (GameObject* eyeBall : mEyeBalls)
    {
        eyeBall->SetEnabled(false);
    }
    mEyeBalls.clear();
}
