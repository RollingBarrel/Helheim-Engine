#include "Katana.h"
#include "Application.h"
#include "AudioManager.h"
#include "GameManager.h"
#include "GameObject.h"
#include "BoxColliderComponent.h"
#include "Enemy.h"
#include "ScriptComponent.h"
#include "GameObject.h"
#include "PlayerController.h"

Katana::Katana(BoxColliderComponent* collider, TrailComponent* trail) : MeleeWeapon(collider, trail)
{
    mDamage = 4.f;
    mEnergyCost = 10;
    mCooldownMultiplier = 0.5f;
    mCombo1st = 0.5f;
    mCombo2nd = 0.5f;
    mComboEnd = 0.5f;
}

Katana::~Katana()
{
}

void Katana::PlayHitSound()
{
    const char* parameterName = "Speed";
    GameManager::GetInstance()->GetAudio()->PlayOneShot(
        SFX::MEELEE,
        GameManager::GetInstance()->GetPlayer()->GetWorldPosition(),
        { { parameterName, 8.0f } }
    );
}

void Katana::HitEffect(GameObject* enemy)
{
    Enemy* enemyScript = static_cast<Enemy*>(static_cast<ScriptComponent*>(enemy->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
    if (enemyScript)
    {
        enemyScript->TakeDamage(mDamage * GameManager::GetInstance()->GetPlayerController()->GetDamageModifier());
    }
}
