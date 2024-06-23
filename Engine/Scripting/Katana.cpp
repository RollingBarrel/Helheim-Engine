#include "Katana.h"
#include "Application.h"
#include "AudioManager.h"
#include "GameManager.h"
#include "GameObject.h"

Katana::Katana(BoxColliderComponent* collider, TrailComponent* trail) : MeleeWeapon(collider, trail)
{
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
