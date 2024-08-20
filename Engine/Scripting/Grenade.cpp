#include "Grenade.h"
#include "GameObject.h"
#include "ModuleScene.h"
#include "ScriptComponent.h"
#include "Enemy.h"
#include <vector>
#include "Application.h"

#include "PoolManager.h"
#include "GameManager.h"
#include "RayCastBullet.h"
#include "ColorGradient.h"
#include "TrailComponent.h"

CREATE(Grenade)
{
	CLASS(owner);
    MEMBER(MemberType::FLOAT, mGrenadeDPS);
    MEMBER(MemberType::FLOAT, mGrenadeDuration);
    MEMBER(MemberType::FLOAT, mGrenadeRadius);
    MEMBER(MemberType::FLOAT, mSpeed);
    MEMBER(MemberType::GAMEOBJECT, mGrenade);
	END_CREATE;
}

Grenade::Grenade(GameObject* owner) : Script(owner)
{
}

Grenade::~Grenade()
{
}

void Grenade::Start()
{

} 

void Grenade::Update()
{
    if (mState == GRENADE_STATE::MOVEMENT)
    {
        MoveToTarget();
    }

    if (mState == GRENADE_STATE::EXPLOSION_START)
    {
        Explosion();
    }
}

void Grenade::MoveToTarget()
{
    mElapsedTime += App->GetDt();

    // Ensure the time does not exceed the total flight time
    if (mElapsedTime > mFlightTime)
    {
        mElapsedTime = mFlightTime;
    }

    mCurrentPosition = CalculatePositionAtTime(mElapsedTime);
    mGrenade->SetWorldPosition(mCurrentPosition);

    if (mElapsedTime >= mFlightTime)
    {
        mGrenade->SetEnabled(false);

        mState = GRENADE_STATE::EXPLOSION_START;
        mGameObject->SetWorldPosition(mDestination);
    }
}

void Grenade::CalculateTrajectory()
{
    float3 displacement = mDestination - mInitialPosition;
    float horizontalDistance = float3(displacement.x, 0, displacement.z).Length();
    float verticalDistance = displacement.y;

    float initialHorizontalSpeed = horizontalDistance / mSpeed;

    mFlightTime = 2 * initialHorizontalSpeed / 9.81;

    float vy = (verticalDistance + 9.81 * mFlightTime * mFlightTime / 2) / mFlightTime;

    float dynamicHeight = mInitialPosition.y + (vy * vy) / (2 * 9.81);

    float vx = displacement.x / mFlightTime;
    float vz = displacement.z / mFlightTime;

    mVelocity = float3(vx, vy, vz);

    mElapsedTime = 0;
}


float3 Grenade::CalculatePositionAtTime(float t)
{
    float x = mVelocity.x * t;
    float z = mVelocity.z * t;
    float y = mInitialPosition.y + mVelocity.y * t - 0.5f * 9.81 * t * t;

    return float3(mInitialPosition.x + x, y, mInitialPosition.z + z);
}

void Grenade::Explosion()
{
    if (!mExplosionTimer.Delay(mGrenadeDuration))
    {
        BlackHole();
    }
    else 
    {
        EndExplosion();
    }
}

void Grenade::BlackHole()
{
    std::vector<GameObject*> affectedEnemies = GetAffectedEnemies();

    PullCloser(affectedEnemies);
}

void Grenade::PullCloser(std::vector<GameObject*> enemies)
{
    for (auto& enemy : enemies)
    {
        float3 direction = mGameObject->GetWorldPosition() - enemy->GetWorldPosition();
        float distance = direction.Length();
        if (distance > 0)
        {
            float3 normalizedDirection = float3(direction.x / distance, direction.y / distance, direction.z / distance);
            float pullStrength = 1.0f * App->GetDt();
            enemy->SetWorldPosition(enemy->GetWorldPosition() + normalizedDirection * pullStrength);

            ScriptComponent* script = (ScriptComponent*)enemy->GetComponent(ComponentType::SCRIPT);
            Enemy* target = (Enemy*)script->GetScriptInstance();

            if (target != nullptr)
            {
                target->TakeDamage(mGrenadeDPS * App->GetDt());
                target->SetAttracted(true);
            }
        }
    }
}

void Grenade::EndExplosion()
{
    mGameObject->SetEnabled(false);
    mState = GRENADE_STATE::INACTIVE;
}

std::vector<GameObject*> Grenade::GetAffectedEnemies()
{
    // Until 
    ModuleScene* scene = App->GetScene();
    const std::vector<GameObject*>& AllEnemies = scene->FindGameObjectsWithTag("Enemy");
    std::vector<GameObject*> AffectedEnemies;
    

    // Check if enemies are inside circle
    for (const auto& e : AllEnemies)
    {
        float3 diff = e->GetWorldPosition() - mGameObject->GetWorldPosition();
        float distanceSquared = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

        if (distanceSquared <= (mGrenadeRadius * mGrenadeRadius))
        {
            AffectedEnemies.push_back(e);
        }
    }

    return AffectedEnemies;
}

void Grenade::SetPositionDestination(float3 initialPosition, float3 destination)
{
    mGameObject->SetEnabled(true);
    mGrenade->SetEnabled(true);

    mState = GRENADE_STATE::MOVEMENT;
    mGrenade->SetWorldPosition(initialPosition);
    
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

