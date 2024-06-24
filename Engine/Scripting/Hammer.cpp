#include "Hammer.h"
#include "Enemy.h"
#include "ScriptComponent.h"
#include "GameObject.h"

Hammer::Hammer(BoxColliderComponent* collider, TrailComponent* trail) : MeleeWeapon(collider, trail)
{
    mDamage = mHeavyDamage;
    mEnergyCost = mHeavyEnergyCost;
    mCooldownMultiplier = mHeavyCooldownMultiplier;
}

Hammer::~Hammer()
{
}

void Hammer::PlayHitSound()
{
}

void Hammer::ApplySpecialEffects(GameObject* enemy)
{
    Enemy* enemyScript = reinterpret_cast<Enemy*>(reinterpret_cast<ScriptComponent*>(enemy->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
    if (enemyScript)
    {
        enemyScript->PushBack();
    }
}

