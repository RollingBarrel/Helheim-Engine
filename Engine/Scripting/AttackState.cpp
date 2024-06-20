#include "AttackState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "PlayerController.h"
#include "Weapon.h"

AttackState::AttackState(PlayerController* player) : State(player)
{
}

AttackState::~AttackState()
{
}

StateType AttackState::HandleInput()
{
    

    if (mAttackTimer < mWeapon->GetAttackTime())
    {
        return StateType::ATTACK;
    }
    else
    {
        mAttackTimer = 0;
        if (mWeapon->GetType() == Weapon::WeaponType::RANGE && mWeapon->GetCurrentAmmo() == 0)
        {
            return StateType::RELOAD;
        }
        if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_DOWN || App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_REPEAT)
        {
            return StateType::ATTACK;
        }
        return StateType::AIM;
    }
}

void AttackState::Update()
{
    if (mAttackTimer == 0) 
    {
        mWeapon->Attack();
        PlayAudio();
    }

    mAttackTimer += App->GetDt();
}

void AttackState::Enter()
{
    mAttackTimer = 0;
    mWeapon = mPlayerController->GetWeapon();
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

void AttackState::PlayAudio()
{

}
