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
    if (mPlayerController->GetPlayerLowerState()->GetType() == StateType::DASH) return StateType::AIM;

    if (App->GetInput()->GetKey(Keys::Keys_E) == KeyState::KEY_UP ||
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == ButtonState::BUTTON_UP)
    {
        mThrowGrenade = true;
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
    mThrowGrenade = false;

    mPlayerController->SetGrenadeVisuals(true);
    //mPlayerController->SetSpineAnimation("tGranade", 0.1f);
}

void GrenadeState::Exit()
{
    mPlayerController->SetGrenadeVisuals(false);
    
    if (mThrowGrenade) 
    {
        mPlayerController->ThrowGrenade();
        GameManager::GetInstance()->GetHud()->SetGrenadeCooldown(mPlayerController->GetGrenadeCooldown());
    }
}

StateType GrenadeState::GetType()
{
    return StateType::GRENADE;
}