#include "Hammer.h"
#include "Enemy.h"
#include "ScriptComponent.h"
#include "GameObject.h"

Hammer::Hammer(BoxColliderComponent* collider, TrailComponent* trail) : MeleeWeapon(collider, trail)
{
    mDamage = 7.f;
    mEnergyCost = 10.f;
    mCooldownMultiplier = 1.5f;
    mCombo1st = 2.f;
    mCombo2nd = 2.f;
    mComboEnd = 2.f;
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

