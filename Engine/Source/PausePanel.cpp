#include "PausePanel.h"
#include "imgui.h"
#include "Application.h"
#include "ModuleScene.h"

#include "Timer.h"

PausePanel::PausePanel() : Panel(PAUSEPANEL, true)
{
	mState = GameState::STOP;
}

void PausePanel::Draw(int windowFlags)
{
	if (mState == GameState::RUN_ONCE) {
		Pause();
	}

	ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(150, 60), ImGuiCond_Once);
	windowFlags |= ImGuiWindowFlags_NoTitleBar;
	ImGui::Begin(GetName(), &mOpen, windowFlags);
	
	if (ImGui::Button("Play", ImVec2(40, 40)) && mState != GameState::PLAYING) {
		Play();
	}
	ImGui::SameLine();
	if (ImGui::Button("Pause", ImVec2(40, 40)) && mState == GameState::PLAYING) {
		Pause();
	}
	ImGui::SameLine();
	if (ImGui::Button("Step", ImVec2(40, 40)) && mState == GameState::PAUSED) {
		Play();
		mState = GameState::RUN_ONCE;
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop", ImVec2(40, 40)) && (mState == GameState::PLAYING || mState == GameState::PAUSED)) {
		Stop();
	}

	ImGui::End();
}

void PausePanel::Play() {
	App->GetEngineClock()->Pause();				//Pauses Engine Clock (variables don't reset)
	App->GetGameClock()->Start();				//Starts GameClock

	App->SetCurrentClock(App->GetGameClock());	//Changes the current clock from Engine to Game

	App->SetUpdateTimer(true);					//The timer must update in this state

	mState = GameState::PLAYING;
	App->GetScene()->Save("TemporalScene");
}

void PausePanel::Pause() {
	App->GetGameClock()->SetTimeScale(0.0f);

	App->SetUpdateTimer(false);				//The timer doesn't update in this state

	mState = GameState::PAUSED;
}

void PausePanel::Stop() {

	//Adds the real time and frames from the game execution to the engine timer
	App->GetEngineClock()->SetTotalTime(App->GetGameClock()->GetRealTime());
	App->GetEngineClock()->SetTotalFrames(App->GetGameClock()->GetTotalFrames());

	App->GetGameClock()->Stop();			//Resets the Game clock time and variables

	App->SetCurrentClock(App->GetEngineClock());	//Changes the current clock from Game to Engine so it updates
	App->GetEngineClock()->Resume();				//Engine clock execution resumes -> mSpeed = 1

	App->SetUpdateTimer(true);					//The engine timer always updates when active

	mState = GameState::STOP;
	App->GetScene()->Load("TemporalScene");
}