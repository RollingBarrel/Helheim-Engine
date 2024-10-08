#include "Bat.h"
#include "Application.h"
#include "Enemy.h"
#include "ScriptComponent.h"
#include "GameObject.h"

#include "GameManager.h"
#include "PlayerController.h"

Bat::Bat(BoxColliderComponent* collider, TrailComponent* trail) : MeleeWeapon(collider, trail)
{
    mDamage = 4.0f;
    mCombo1st = 1.0f;
    mCombo2nd = 0.4f;
    mComboEnd = 0.4f;
}

Bat::~Bat()
{
}

void Bat::PlayHitSound()
{
}

void Bat::HitEffect(GameObject* enemy)
{
    Enemy* enemyScript = static_cast<Enemy*>(static_cast<ScriptComponent*>(enemy->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
    if (enemyScript)
    {
        enemyScript->TakeDamage(mDamage * GameManager::GetInstance()->GetPlayerController()->GetDamageModifier());
    }
    LOG("apply special effects bat");
}
