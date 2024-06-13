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
	return StateType::AIM;
}

void AttackState::Update()
{
    Weapon* weapon = mPlayerController->GetWeapon();
    weapon->BasicAttack();

    PlayAudio();
}

void AttackState::Enter()
{
    //TODO: Different states and animations depending on the weapon
    mPlayerController->SetSpineAnimation("tAttack", 0.1f);
}

void AttackState::Exit()
{
}

StateType AttackState::GetType()
{
    return StateType::ATTACK;
}

void AttackState::PlayAudio()
{

}
