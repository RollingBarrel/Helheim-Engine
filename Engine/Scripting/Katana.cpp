#include "Katana.h"
#include "Application.h"
#include "AudioManager.h"
#include "GameManager.h"
#include "GameObject.h"
#include "BoxColliderComponent.h"

Katana::Katana(BoxColliderComponent* collider, TrailComponent* trail) : MeleeWeapon(collider, trail)
{
    mDamage = 4.f;
    mEnergyCost = 10.f;
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
        GameManager::GetInstance()->GetPlayer()->GetPosition(),
        { { parameterName, 8.0f } }
    );
}

void Katana::ApplySpecialEffects(GameObject* enemy)
{
    if (mCollider)
    {
		mCollider->SetSize(mCollider->GetSize() * 1.4f);
	}
}
