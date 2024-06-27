#include "Bat.h"
#include "Application.h"
#include "Enemy.h"
#include "ScriptComponent.h"
#include "GameObject.h"

Bat::Bat(BoxColliderComponent* collider, TrailComponent* trail) : MeleeWeapon(collider, trail)
{
    mDamage = 4.0f;
    mCombo1st = 0.4f;
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
    Enemy* enemyScript = reinterpret_cast<Enemy*>(reinterpret_cast<ScriptComponent*>(enemy->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
    if (enemyScript)
    {
        enemyScript->TakeDamage(mDamage);
    }
    LOG("apply special effects bat");
}


