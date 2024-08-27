#include "GrenadeState.h"

#include "GameManager.h"
#include "HudController.h"
#include "Application.h"
#include "ModuleInput.h"
#include "PlayerController.h"
#include "Keys.h"
#include "ScriptComponent.h"
#include "Grenade.h"
#include "AttackState.h"
#include "SpecialState.h"
#include "SwitchState.h"
#include "ReloadState.h"
#include "UltimateState.h"

GrenadeState::GrenadeState(PlayerController* player, float cooldown) : State(player, cooldown)
{
}

GrenadeState::~GrenadeState()
{
}

StateType GrenadeState::HandleInput()
{
    if (mPlayerController->GetPlayerLowerState()->GetType() == StateType::DASH) return StateType::AIM;

    if (mPlayerController->GetAttackState()->IsReady())
    {
        return StateType::ATTACK;
    }

    if (mPlayerController->GetSpecialState()->IsReady())
    {
        mPlayerController->GetSpecialState()->ResetCooldown();
        return StateType::SPECIAL;
    }

    if (mPlayerController->GetSwitchState()->IsReady() &&
        (App->GetInput()->GetKey(Keys::Keys_Q) == KeyState::KEY_DOWN ||
            App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_Y) == ButtonState::BUTTON_DOWN))
    {
        mPlayerController->GetSwitchState()->ResetCooldown();
        return StateType::SWITCH;
    }

    if (mPlayerController->GetReloadState()->IsReady() &&
        (App->GetInput()->GetKey(Keys::Keys_R) == KeyState::KEY_DOWN ||
            App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_X) == ButtonState::BUTTON_DOWN))
    {
        mPlayerController->GetReloadState()->ResetCooldown();
        return StateType::RELOAD;
    }

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
    mPlayerController->SetSpineAnimation("tGrenade", 0.3f);
}

void GrenadeState::Exit()
{
    mPlayerController->SetGrenadeVisuals(false);
    
    if (mThrowGrenade) 
    {
        mPlayerController->ThrowGrenade();
        if (GameManager::GetInstance()->GetHud())
        {
            GameManager::GetInstance()->GetHud()->SetGrenadeCooldown(mPlayerController->GetGrenadeCooldown());
        }
    }
}

StateType GrenadeState::GetType()
{
    return StateType::GRENADE;
}