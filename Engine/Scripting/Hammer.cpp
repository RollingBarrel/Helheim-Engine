#include "Hammer.h"
#include "Application.h"
#include "AudioManager.h"
#include "GameManager.h"
#include "GameObject.h"
Hammer::Hammer()
{
    mDamage = 7.0f;
}

Hammer::~Hammer()
{
}

void Hammer::Enter()
{
}

void Hammer::Attack(float time)
{
    LOG("Hammer Attack");
    PlayHitSound();
}

void Hammer::Exit()
{
}

void Hammer::PlayHitSound()
{
    const char* parameterName = "Speed";
    GameManager::GetInstance()->GetAudio()->PlayOneShot(
        SFX::MEELEE,
        GameManager::GetInstance()->GetPlayer()->GetPosition(),
        { { parameterName, 4.0f } }
    );
}
