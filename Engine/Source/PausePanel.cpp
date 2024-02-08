#include "PausePanel.h"
#include "imgui.h"
#include "Application.h"
//#include "ModuleTimer.h"
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
	
	if (ImGui::Button("Play", ImVec2(40, 40))) {
		Play();
	}
	ImGui::SameLine();
	if (ImGui::Button("Pause", ImVec2(40, 40))) {
		Pause();
	}
	ImGui::SameLine();
	if (ImGui::Button("Step", ImVec2(40, 40))) {
		Play();
		mState = GameState::RUN_ONCE;
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop", ImVec2(40, 40)) && mState == GameState::PLAYING) {
		Stop();
	}

	ImGui::End();
}

void PausePanel::Play() {
	App->GetEngineClock()->SetTimeScale(1.0f);
	mState = GameState::PLAYING;
	App->GetScene()->Save("TemporalScene");
}

void PausePanel::Pause() {
	App->GetEngineClock()->SetTimeScale(0.0f);
	mState = GameState::PAUSED;
}

void PausePanel::Stop() {
	App->GetEngineClock()->SetTimeScale(1.0f);
	App->GetScene()->Load("TemporalScene");
}