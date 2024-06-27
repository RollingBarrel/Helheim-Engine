#include "AttackState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "PlayerController.h"
#include "Weapon.h"
#include "ModuleDetourNavigation.h"
#include "MeleeWeapon.h"


AttackState::AttackState(PlayerController* player, float cooldown) : State(player, cooldown)
{
}

AttackState::~AttackState()
{
}

StateType AttackState::HandleInput()
{
    if (mPlayerController->GetPlayerLowerState()->GetType() == StateType::DASH) return StateType::AIM;

    mAttackTimer += App->GetDt();
    if (mAttackTimer < mWeapon->GetAttackDuration())
    {
        // MOVE TO WEAPON
        if (mWeapon->GetType() == Weapon::WeaponType::MELEE and
            App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_DOWN)
        {
            reinterpret_cast<MeleeWeapon*>(mWeapon)->IncreaseComboStep();
        } 
        return StateType::ATTACK;
    }
       
    return StateType::AIM;
}

void AttackState::Update()
{
    mWeapon->Attack(mAttackTimer);
}

void AttackState::Enter()
{
    mAttackTimer = 0.0f;
    mWeapon = mPlayerController->GetWeapon();
    if (mPlayerController->GetWeapon()->GetType() == Weapon::WeaponType::RANGE)
        mPlayerController->SetSpineAnimation("tAttackRanged", 0.3f);
    else
        mPlayerController->SetSpineAnimation("tAttackMelee", 0.3f);
    mWeapon->Enter();
}

void AttackState::Exit()
{

    mWeapon->Exit();
    mWeapon = nullptr;
}

StateType AttackState::GetType()
{
    return StateType::ATTACK;
}

bool AttackState::IsReady()
{
    mStateTimer += App->GetDt();
    if (mStateTimer >= mStateCooldown) {
        if (mPlayerController->GetWeapon()->GetType() == Weapon::WeaponType::RANGE && mPlayerController->GetWeapon()->GetCurrentAmmo() != 0)
        {
            return true;
        }
        if (mPlayerController->GetWeapon()->GetType() == Weapon::WeaponType::MELEE)
        {
            return true;
        }
    }
    return false;
}
