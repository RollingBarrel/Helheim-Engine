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
}

void BossLaser::SpawnEyeBalls()
{
    std::vector<float3> positions = {
        float3(1, 0, 1),
        float3(-1, 0, 1),
        float3(1, 0, -1),
        float3(-1, 0, -1),
        float3(0, 0, 2)
    };

    for (const auto& pos : positions)
    {
        GameObject* eyeBall = App->GetScene()->InstantiatePrefab("Eye.prefab", mGameObject);
        if (eyeBall)
        {
            eyeBall->SetWorldPosition(mGameObject->GetWorldPosition() + pos);
            BossLaserEyeBall* eyeBallScript = static_cast<BossLaserEyeBall*>(static_cast<ScriptComponent*>(eyeBall->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
            if (eyeBallScript)
            {
                eyeBallScript->Init(mDamage, mDistance, mLaserDuration, mEyeRotationSpeed);
            }
            mEyeBalls.push_back(eyeBall);
        }
    }
}