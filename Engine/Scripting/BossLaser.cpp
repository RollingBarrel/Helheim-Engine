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

    // Play VFX energy coming from the ground to the torso.
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
    float posAngles[] = { -45.0f, -90.0f, -0.0f, -270.0f, -315.0f };
    float attackAngles[] = { 90.f, 90.f,  90.f, 45.f, 45.f };
    float rotationDirections[] = { 1.0f, 1.0f, 0.f, -1.0f, -1.0f };  

    for (size_t i = 0; i < mPoolSize; ++i)
    {
        if (i >= mEyeBallPool.size())
            break;

        GameObject* eyeBall = mEyeBallPool[i];

        float posAngle = posAngles[i];
        float attackAngle = attackAngles[i];
        float rotationDirection = rotationDirections[i];  

        float radians = DegToRad(posAngle);

        float x = std::cos(radians) * mDistance;
        float z = std::sin(radians) * mDistance;

        float3 positionOffset(x, -2, z);

        eyeBall->SetWorldPosition(mGameObject->GetWorldPosition() + positionOffset);
        eyeBall->SetEnabled(true);

        BossLaserEyeBall* eyeBallScript = static_cast<BossLaserEyeBall*>(static_cast<ScriptComponent*>(eyeBall->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
        if (eyeBallScript)
        {
            eyeBallScript->Init(mDistance, mLaserEnemyDuration, attackAngle, rotationDirection);
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