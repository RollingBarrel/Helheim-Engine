#include "Grenade.h"
#include "Application.h"

#include "GameObject.h"
#include "ScriptComponent.h"

#include "GameManager.h"
#include "AudioManager.h"
#include "ModuleScene.h"
#include "Enemy.h"
#include "PlayerCamera.h"


CREATE(Grenade)
{
	CLASS(owner);
    MEMBER(MemberType::FLOAT, mGrenadeDamage);
    MEMBER(MemberType::FLOAT, mBlackHoleDuration);
    MEMBER(MemberType::FLOAT, mExplosionDuration);
    MEMBER(MemberType::FLOAT, mGrenadeRadius);
    MEMBER(MemberType::FLOAT, mTrajectorySpeedFactor);
    MEMBER(MemberType::GAMEOBJECT, mTrail);
    MEMBER(MemberType::GAMEOBJECT, mExplosionSFX);
    MEMBER(MemberType::GAMEOBJECT, mBlackHoleSFX);
    MEMBER(MemberType::GAMEOBJECT, mSphere);
	END_CREATE;
}

void Grenade::Start()
{
    mBlackHoleSFX->SetEnabled(false);
    mExplosionSFX->SetEnabled(false);
    mTrail->SetEnabled(false);
    mSphere->SetEnabled(false);
}

void Grenade::Update()
{   
    if (GameManager::GetInstance()->IsPaused()) return;

    switch (mState)
    {
    case GRENADE_STATE::MOVE:
        MoveToTarget();
        break;
    case GRENADE_STATE::BLACK_HOLE:
        BlackHole();
        break;
    case GRENADE_STATE::EXPLOSION:
        Explosion();
        break;
    default:
        break;
    }
}

void Grenade::MoveToTarget()
{
    // Increment elapsed time, scaled by the trajectory speed factor
    mElapsedTime += App->GetDt() * mTrajectorySpeedFactor;

    // Ensure the time does not exceed the total flight time
    if (mElapsedTime > mFlightTime) mElapsedTime = mFlightTime;

    mCurrentPosition = CalculatePositionAtTime(mElapsedTime);
    mTrail->SetWorldPosition(mCurrentPosition);

    if (mElapsedTime >= mFlightTime)
    {
        mBlackHoleSFX->SetEnabled(false);
        mBlackHoleSFX->SetEnabled(true);
        mSphere->SetEnabled(true);

        mState = GRENADE_STATE::BLACK_HOLE;
        //mBlackHoleSFX->SetWorldPosition(mDestination + float3(0, 0.8, 0) );
        mExplosionSFX->SetWorldPosition(mDestination);
        mSphere->SetLocalScale(float3::zero);
        mSphere->SetWorldPosition(mDestination);

        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::PLAYER_BLACKHOLE1, mExplosionSFX->GetWorldPosition());
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::PLAYER_BLACKHOLE2, mExplosionSFX->GetWorldPosition());
    }
}

void Grenade::CalculateTrajectory()
{
    float3 displacement = mDestination - mInitialPosition;

    float horizontalDistance = float3(displacement.x, 0.0f, displacement.z).Length();
    float verticalDistance = displacement.y;

    mFlightTime = 2.0f * horizontalDistance / 9.81f;

    float vy = (verticalDistance + 9.81f * mFlightTime * mFlightTime / 2.0f) / mFlightTime;

    float vx = displacement.x / mFlightTime;
    float vz = displacement.z / mFlightTime;

    mVelocity = float3(vx, vy, vz);

    // Reset elapsed time
    mElapsedTime = 0.0f;
}

float3 Grenade::CalculatePositionAtTime(float t)
{
    float x = mVelocity.x * t;
    float z = mVelocity.z * t;
    float y = mInitialPosition.y + mVelocity.y * t - 0.5f * 9.81f * t * t;

    return float3(mInitialPosition.x + x, y, mInitialPosition.z + z);
}

void Grenade::Explosion()
{
    if (mExplosionTimer.Delay(mExplosionDuration)) 
    {
        EndExplosion();
    }
    else mExplosionSFX->SetEnabled(true);
    //mSphere->SetLocalScale(float3(10.0f));
}

void Grenade::BlackHole()
{
    std::vector<GameObject*> affectedEnemies;
    GetAffectedEnemies(affectedEnemies);
    PullCloser(affectedEnemies);

    if (mBlackHoleTimer.Delay(mBlackHoleDuration))
    {
        mState = GRENADE_STATE::EXPLOSION;
        mTrail->SetEnabled(false);
        MakeDamage(affectedEnemies);

    }

    float scaleInTime = (mBlackHoleTimer.GetTimePassed() - mBlackHoleDuration / 2.0f) / (mBlackHoleDuration / 2.0f);
    scaleInTime = - scaleInTime * scaleInTime + 1.0f;
    mSphere->SetLocalScale(float3(scaleInTime));
}

void Grenade::MakeDamage(std::vector<GameObject*> enemies)
{
    GameManager::GetInstance()->GetPlayerCamera()->ActivateShake(0.5f, 0.2f);
    for (GameObject* enemy : enemies)
    {
        ScriptComponent* script = static_cast<ScriptComponent*>(enemy->GetComponent(ComponentType::SCRIPT));
        Enemy* target = static_cast<Enemy*>(script->GetScriptInstance());
        target->TakeDamage(mGrenadeDamage);
    }
}

void Grenade::PullCloser(std::vector<GameObject*> enemies)
{
    for (auto& enemy : enemies)
    {
        float3 direction = mExplosionSFX->GetWorldPosition() - enemy->GetWorldPosition();
        float distance = direction.Length();
        if (distance > 0)
        {
            direction.y = 0;
            float3 normalizedDirection = direction.Normalized();
            float pullStrength = 3.0f * App->GetDt();

            //Patch to solve grenade not attracting correctly barrels
            if (enemy->GetName().compare("OB_explosive_Barrell") != 0)
                enemy->SetWorldPosition(enemy->GetWorldPosition() + normalizedDirection * pullStrength);
            else
                enemy->GetParent()->SetWorldPosition(enemy->GetParent()->GetWorldPosition() + normalizedDirection * pullStrength);

            ScriptComponent* script = static_cast<ScriptComponent*>(enemy->GetComponent(ComponentType::SCRIPT));
            Enemy* target = static_cast<Enemy*>(script->GetScriptInstance());

            if (target)
            {
                target->SetAttracted(true);
            }
        }
    }
}

void Grenade::EndExplosion()
{
    mState = GRENADE_STATE::NONE;
    mSphere->SetEnabled(false);
}

void Grenade::GetAffectedEnemies(std::vector<GameObject*>& affectedEnemies) const
{
    float radiusSq = mGrenadeRadius * mGrenadeRadius;

    const std::vector<GameObject*>& allEnemies = App->GetScene()->FindGameObjectsWithTag("Enemy");
    for (GameObject* enemy : allEnemies)
    {
        float distanceSquared = enemy->GetWorldPosition().DistanceSq(mExplosionSFX->GetWorldPosition());

        if (distanceSquared < radiusSq)
        {
            affectedEnemies.push_back(enemy);
        }
    }
}

void Grenade::ThrowGrenade(float3 initialPosition, float3 destination)
{
    mTrail->SetEnabled(true);
    mSphere->SetEnabled(false);
    mBlackHoleSFX->SetEnabled(false);
    mExplosionSFX->SetEnabled(false);

    mState = GRENADE_STATE::MOVE;
    mTrail->SetWorldPosition(initialPosition);
    
    mInitialPosition = initialPosition;
    mDestination = destination;

    mCurrentPosition = mInitialPosition;
    mTotalDistance = mInitialPosition.Distance(mDestination);

    CalculateTrajectory();
}

float Grenade::GetGrenadeRadius()
{
    return mGrenadeRadius;
}

