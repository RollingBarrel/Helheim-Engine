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

	if (!mReloaded) 
	{
		return StateType::RELOAD;
	}
		
	return StateType::AIM;
}

void ReloadState::Update()
{
	mReloadTimer += App->GetDt();
	if (mReloadTimer > mPlayerController->GetReloadDuration()) 
	{
		mReloaded = true;
		mPlayerController->Reload();
		LOG("Reloaded!");
	}
}

void ReloadState::Enter()
{
	mPlayerController->SetSpineAnimation("tReload", 0.1f);

	mReloadTimer = 0;
	mReloaded = false;
}

void ReloadState::Exit()
{
}

StateType ReloadState::GetType()
{
	return StateType::RELOAD;
}

void ReloadState::PlayAudio()
{
}