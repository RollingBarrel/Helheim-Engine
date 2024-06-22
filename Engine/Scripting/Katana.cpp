#include "Katana.h"
#include "Application.h"
#include "AudioManager.h"
#include "GameManager.h"
#include "GameObject.h"

Katana::Katana()
{
}

Katana::~Katana()
{
}

void Katana::Enter()
{
}

void Katana::Attack(float time)
{
	LOG("Katana Attack");
    
    PlayHitSound();
}

void Katana::Exit()
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
