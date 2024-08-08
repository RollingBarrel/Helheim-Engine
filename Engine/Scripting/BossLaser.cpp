#include "BossLaser.h"
#include "Application.h"
#include "GameObject.h"
#include "BoxColliderComponent.h"
#include "ModuleScene.h"
#include "ScriptComponent.h"
#include "PlayerController.h"
#include "GameManager.h"
#include "MathFunc.h"
#include "State.h"
#include "Geometry/Ray.h"
#include "BossLaserEyeBall.h"

CREATE(BossLaser)
{
	CLASS(owner);
	MEMBER(MemberType::FLOAT, mDamage);

	END_CREATE;
}

BossLaser::BossLaser(GameObject* owner) : Script(owner)
{
}

void BossLaser::Start()
{
    mCurrentState = LaserState::IDLE;

    // Initialize the pool
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
        if (mStateTime >= mLaserDuration)
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
	mDamage = damage;
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
    // Destroy or disable the eyeballs
    for (GameObject* eyeBall : mEyeBalls)
    {
        eyeBall->SetEnabled(false); // or Destroy(eyeBall)
    }
    mEyeBalls.clear();

    ReturnEyeBallsToPool();

}

void BossLaser::SpawnEyeBalls()
{
    std::vector<float3> positions = {
        float3(4, -2, 4),
        float3(-4, -2, 4),
        float3(4, -2, -4),
        float3(-4, -2, -4),
        float3(0, -2, 4)
    };

    size_t index = 0;
    for (const auto& pos : positions)
    {
        if (index >= mEyeBallPool.size())
            break;

        GameObject* eyeBall = mEyeBallPool[index++];
        eyeBall->SetWorldPosition(mGameObject->GetWorldPosition() + pos);
        eyeBall->SetEnabled(true);

        BossLaserEyeBall* eyeBallScript = static_cast<BossLaserEyeBall*>(static_cast<ScriptComponent*>(eyeBall->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
        if (eyeBallScript)
        {
            eyeBallScript->Init(mDamage, mDistance, mLaserDuration, mEyeRotationSpeed);
        }

        mEyeBalls.push_back(eyeBall);
    }
}

void BossLaser::ReturnEyeBallsToPool()
{
    for (GameObject* eyeBall : mEyeBalls)
    {
        eyeBall->SetEnabled(false);
        // Optionally reset any other properties if needed
    }
    mEyeBalls.clear();
}