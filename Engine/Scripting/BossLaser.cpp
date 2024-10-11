#include "BossLaser.h"
#include "Application.h"
#include "GameObject.h"
#include "ModuleScene.h"
#include "ScriptComponent.h"
#include "GameManager.h"
#include "AudioManager.h"
#include "BossLaserEyeBall.h"

CREATE(BossLaser)
{
    CLASS(owner);
    END_CREATE;
}

BossLaser::BossLaser(GameObject* owner) : Script(owner)
{
}

void BossLaser::Start()
{
    mCurrentState = LaserState::IDLE;

    // Instantiate a single laser eyeball
    mLaserEyeBall = App->GetScene()->InstantiatePrefab("BossLaser_EyeBall.prfb", mGameObject);
    if (mLaserEyeBall)
    {
        mLaserEyeBall->SetEnabled(false);
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

void BossLaser::Init(float damage, float duration, float distance, float speed)
{
    mDamage = damage;
    mLaserDuration = duration - mChargeTime;
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

    if (mLaserEyeBall)
    {
        mLaserEyeBall->SetWorldPosition(mGameObject->GetWorldPosition());
        mLaserEyeBall->SetLocalPosition(float3(-0.1, 0.71, 0.58));
        mLaserEyeBall->SetEnabled(true);
        BossLaserEyeBall* eyeBallScript = static_cast<BossLaserEyeBall*>(static_cast<ScriptComponent*>(mLaserEyeBall->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
        if (eyeBallScript)
        {
            eyeBallScript->Init(mDamage, mLaserDuration, mLaserDistance, mLaserSpeed, -90.0f);

            mLaserSound = GameManager::GetInstance()->GetAudio()->Play(SFX::BOSS_LASER);
            GameManager::GetInstance()->GetAudio()->SetPosition(SFX::BOSS_LASER, mLaserSound, mLaserEyeBall->GetWorldPosition());
        }
    }
}

void BossLaser::Cooldown()
{
    mCurrentState = LaserState::COOLDOWN;
    mStateTime = 0.0f;

    if (mLaserEyeBall)
    {
        if (mLaserSound != -1)
        {
            mLaserSound = GameManager::GetInstance()->GetAudio()->Release(SFX::BOSS_LASER, mLaserSound);
        }

        mLaserEyeBall->SetEnabled(false);
    }
}

void BossLaser::Interrupt()
{
    if (mCurrentState != LaserState::IDLE && mCurrentState != LaserState::COOLDOWN)
    {
        Cooldown();
    }
}
