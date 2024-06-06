#include "ReloadState.h"

#include "Application.h"
#include "PlayerController.h"

ReloadState::ReloadState(PlayerController* player) : State(player)
{
}

ReloadState::~ReloadState()
{
}

StateType ReloadState::HandleInput()
{
	// TODO: Dash cancel reload

	mReloadTimer += App->GetDt();
	if (mReloadTimer <= mPlayerController->GetReloadDuration() && !mReloaded) return StateType::RELOAD;

	return StateType::AIM;
}

void ReloadState::Update()
{
	if (mReloadTimer > mPlayerController->GetReloadDuration()) 
	{
		mReloaded = true;
		mPlayerController->Reload();
	}
}

void ReloadState::Enter()
{
	mReloadTimer = 0;
	mReloaded = false;
}

void ReloadState::Exit()
{
}
