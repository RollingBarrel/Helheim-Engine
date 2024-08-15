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

CREATE(Grenade)
{
	CLASS(owner);
    MEMBER(MemberType::FLOAT, mGrenadeDPS);
    MEMBER(MemberType::FLOAT, mGrenadeDuration);
    MEMBER(MemberType::FLOAT, mGrenadeRadius);
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
    mFire = App->GetScene()->InstantiatePrefab("PistolFire.prfb");
    if (mFire)	mFire->SetEnabled(false);
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

void Grenade::SetPositionDestination(float3 initialPosition, float3 Destination)
{
    mGameObject->SetEnabled(true);

    GameObject* bullet = GameManager::GetInstance()->GetPoolManager()->Spawn(PoolType::BULLET);

    RayCastBullet* bulletScript = reinterpret_cast<RayCastBullet*>(reinterpret_cast<ScriptComponent*>(bullet->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());

    //bulletScript->Init
    ColorGradient gradient;
    gradient.AddColorGradientMark(0.1f, float4(0.0f, 1.0f, 0.0f, 1.0f));
    bullet->SetEnabled(false);
    bulletScript->Init(initialPosition, Destination, mSpeed, 10.0f, true, &gradient);

    mState = GRENADE_STATE::MOVEMENT;
    mFire->SetEnabled(true);
    mDestination = Destination;
    mInitialPosition = initialPosition;

    mCurrentPosition = mInitialPosition;
    mTotalDistance = mInitialPosition.Distance(mDestination);
    mCurrentDistance = 0;


}

float Grenade::GetGrenadeRadius()
{
    return mGrenadeRadius;
}

