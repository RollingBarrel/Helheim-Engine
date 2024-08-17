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

    if (mState == GRENADE_STATE::EXPLOTION_START)
    {
        Explotion();
    }
}

void Grenade::MoveToTarget()
{
    mElapsedTime += App->GetDt();

    // 确保时间不超过总飞行时间
    if (mElapsedTime > mFlightTime)
    {
        mElapsedTime = mFlightTime;
    }

    // 计算当前位置
    mCurrentPosition = CalculatePositionAtTime(mElapsedTime);

    // 更新物体的位置 (在你的框架中实际更新对象的位置)
    // mGameObject->SetPosition(mCurrentPosition);
    mGrenade->SetWorldPosition(mCurrentPosition);

    // 如果到达目标点，停止更新
    if (mElapsedTime >= mFlightTime)
    {
        mState = GRENADE_STATE::EXPLOTION_START;
        mGameObject->SetWorldPosition(mDestination);
    }

    //if (mCurrentPosition.Distance(mDestination) < mThreshold)
    //{
    //    mState = GRENADE_STATE::EXPLOTION_START;
    //    mGameObject->SetWorldPosition(mDestination);
    //    return;
    //}

    //float step = mSpeed * App->GetDt();
    //mCurrentDistance += step;

    //float3 direction = (mDestination - mInitialPosition).Normalized();
    //float3 horizontalPosition = mInitialPosition + direction * mCurrentDistance;

    //// Calculate the height of the arc at the current distance
    //float t = mCurrentDistance / mTotalDistance;
    //float height = 4 * mArcHeight * t * (1 - t);

    //// Update current position
    //mCurrentPosition = float3(horizontalPosition.x, mInitialPosition.y + height, horizontalPosition.z);

    //// Move the object in the scene
    //mFire->SetWorldPosition(mCurrentPosition);
    //transform.position = mCurrentPosition;

    //// Calculate the total distance
    //float3 direction = mDestination - mInitialPosition;
    //float totalDistance = mInitialPosition.Distance(mDestination);

    //// Normalize the direction
    ////float3 normalizedDirection = direction * (1.0f / totalDistance);(
    //float3 normalizedDirection = direction.Normalized();

    //// Calculate the time step based on total speed
    //float distanceTraveled = mSpeed * (1.0f / totalDistance); // Fraction of total distance traveled per frame

    //// Calculate the new position
    //float3 newPosition = mCurrentPosition + (normalizedDirection * distanceTraveled * totalDistance) * App->GetDt();

    //// Calculate the height of the arc at this point
    //float t = mCurrentPosition.Distance(mInitialPosition) / totalDistance;
    //float height = 4 * mArcHeight * t * (1 - t); // Parabolic arc

    //newPosition.y = mInitialPosition.y + (mDestination.y - mInitialPosition.y) * t + height;

    //// Update the current position
    //mCurrentPosition = newPosition;

    //// Set the new world position
    //mFire->SetWorldPosition(mCurrentPosition);

    //// Check if we are within the threshold
    //if (mCurrentPosition.Distance(mDestination) < mThreshold) {
    //    mState = GRENADE_STATE::EXPLOTION_START;
    //    mGameObject->SetWorldPosition(mDestination);
    //}
}

void Grenade::CalculateTrajectory()
{
    // 计算水平和竖直位移
    float3 displacement = mDestination - mInitialPosition;
    float verticalDistance = displacement.y;

    // 计算竖直方向的初始速度（根据设定的最高点高度）
    float vy = std::sqrt(2 * mGravity * mArcHeight);

    // 计算到达最高点和总飞行时间
    float timeToMaxHeight = vy / mGravity;
    float totalFlightTime = timeToMaxHeight + std::sqrt(2 * (mArcHeight - verticalDistance) / mGravity);

    // 水平方向的速度
    float vx = displacement.x / totalFlightTime;
    float vz = displacement.z / totalFlightTime;

    // 设置初速度矢量
    mVelocity = float3(vx, vy, vz);
    mFlightTime = totalFlightTime;
    mElapsedTime = 0.0f; // 重置时间
}

float3 Grenade::CalculatePositionAtTime(float t)
{
    float x = mVelocity.x * t;
    float z = mVelocity.z * t;
    float y = mInitialPosition.y + mVelocity.y * t - 0.5f * mGravity * t * t;

    return float3(mInitialPosition.x + x, y, mInitialPosition.z + z);
}

void Grenade::Explotion()
{
    if (mGrenadeCurrentTime > 0)
    {
        mGrenadeCurrentTime -= App->GetDt();
        BlackHole();
        if (mGrenadeCurrentTime <= 0)
        {
            EndExplotion();  
        }
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

            target->TakeDamage(mGrenadeDPS * App->GetDt());
            target->SetAttracted(true);
        }
    }
}

void Grenade::EndExplotion()
{
    mGameObject->SetEnabled(false);
    mGrenadeCurrentTime = mGrenadeDuration;
    mState = GRENADE_STATE::EXPLOTION_END;
}

std::vector<GameObject*> Grenade::GetAffectedEnemies()
{
    // Until 
    ModuleScene* scene = App->GetScene();
    const std::vector<GameObject*>& AllEnemies = scene->FindGameObjectsWithTag("Enemy");
    std::vector<GameObject*> AffectedEnemies;
    

    // Check if enemies are inside circle
    // TODO: Check hit with physic
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

    mState = GRENADE_STATE::MOVEMENT;
    mGrenade->SetWorldPosition(initialPosition);
    mFire->SetEnabled(true);
    
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

