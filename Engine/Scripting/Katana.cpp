#include "Katana.h"
#include "Application.h"
#include "AudioManager.h"
#include "GameManager.h"
#include "GameObject.h"
#include "BoxColliderComponent.h"

Katana::Katana(BoxColliderComponent* collider, TrailComponent* trail) : MeleeWeapon(collider, trail)
{
    mDamage = mFastDamage;
    mEnergyCost = mFastEnergyCost;
    mCooldownMultiplier = mFastCooldownMultiplier;
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
