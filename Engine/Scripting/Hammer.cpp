#include "Hammer.h"
#include "Enemy.h"
#include "ScriptComponent.h"
#include "GameObject.h"
#include "GameManager.h"
#include "PlayerController.h"

Hammer::Hammer(BoxColliderComponent* collider, TrailComponent* trail) : MeleeWeapon(collider, trail)
{
    mDamage = 7.f;
    mEnergyCost = 10;
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

void Hammer::HitEffect(GameObject* enemy)
{
    Enemy* enemyScript = static_cast<Enemy*>(static_cast<ScriptComponent*>(enemy->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
    if (enemyScript)
    {
        enemyScript->TakeDamage(mDamage * GameManager::GetInstance()->GetPlayerController()->GetDamageModifier());
        enemyScript->PushBack();
    }
}

