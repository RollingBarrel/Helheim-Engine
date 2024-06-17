#include "GrenadeState.h"

#include "GameManager.h"
#include "HudController.h"
#include "Application.h"
#include "ModuleInput.h"
#include "PlayerController.h"
#include "Keys.h"
#include "ScriptComponent.h"
#include "Grenade.h"

GrenadeState::GrenadeState(PlayerController* player) : State(player)
{
}

GrenadeState::~GrenadeState()
{
}

StateType GrenadeState::HandleInput()
{
    if (App->GetInput()->GetKey(Keys::Keys_E) == KeyState::KEY_UP)
    {
        return StateType::AIM;
    }

    return StateType::GRENADE;
}

void GrenadeState::Update()
{
    mPlayerController->UpdateGrenadeVisuals();
}

void GrenadeState::Enter()
{
    //mPlayerController->SetSpineAnimation("tGranade", 0.1f);

   //GameManager::GetInstance()->GetHud()->SetGrenadeCooldown(mPlayerController->GetGrenadeCooldown());
    mPlayerController->SetGrenadeVisuals(true);
}

void GrenadeState::Exit()
{
    mPlayerController->SetGrenadeVisuals(false);
    mPlayerController->ThrowGrenade();
    GameManager::GetInstance()->GetHud()->SetGrenadeCooldown(mPlayerController->GetGrenadeCooldown());
}

StateType GrenadeState::GetType()
{
    return StateType::GRENADE;
}

void GrenadeState::PlayAudio()
{
}