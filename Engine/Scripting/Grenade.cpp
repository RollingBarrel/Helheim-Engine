#include "Grenade.h"
#include "GameObject.h"
#include "ModuleScene.h"
#include "ScriptComponent.h"
#include "Enemy.h"
#include <vector>
#include "Application.h"

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
	
} 

void Grenade::Update()
{
	Explotion();
}

void Grenade::Explotion()
{
    if (mExplotionStart)
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
        float3 direction = mGameObject->GetPosition() - enemy->GetPosition();
        float distance = direction.Length();
        if (distance > 0)
        {
            float3 normalizedDirection = float3(direction.x / distance, direction.y / distance, direction.z / distance);
            float pullStrength = 1.0f * App->GetDt();
            enemy->SetPosition(enemy->GetPosition() + normalizedDirection * pullStrength);

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
    mExplotionStart = false;
}

std::vector<GameObject*> Grenade::GetAffectedEnemies()
{
    // Until 
    ModuleScene* scene = App->GetScene();
    std::vector<GameObject*> AllEnemies;
    std::vector<GameObject*> AffectedEnemies;
    scene->FindGameObjectsWithTag(scene->GetTagByName("Enemy")->GetID(), AllEnemies);

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

void Grenade::SetDestionation(float3 destination)
{
	mDestination = destination;

	mExplotionStart = true;
}

float Grenade::GetGrenadeRadius()
{
    return mGrenadeRadius;
}

