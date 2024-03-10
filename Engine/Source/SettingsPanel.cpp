#include "SettingsPanel.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleDebugDraw.h"
#include "Quadtree.h"
#include "Timer.h"


#include "imgui.h"

SettingsPanel::SettingsPanel() : Panel(SETTINGSPANEL, false)
{
}

SettingsPanel::~SettingsPanel()
{
}

void SettingsPanel::Draw(int windowFlags) {
	if (ImGui::Begin(GetName(), &mOpen, windowFlags))
	{
		ImGui::SeparatorText("Graphic settings");
		bool culling = App->GetScene()->GetApplyFrustumCulling();
		if (ImGui::Checkbox("Apply frustum culling", &culling)) {
			App->GetScene()->SetApplyFrustumCulling(culling);
		}
		ImGui::Indent();
		ImGui::SeparatorText("Engine");
		bool engineVsyncEnabled = App->GetEngineClock()->GetVsyncStatus();
		ImGui::Checkbox("Engine Vsync enabled", &engineVsyncEnabled);
		if (engineVsyncEnabled != App->GetEngineClock()->GetVsyncStatus()) {
			App->GetEngineClock()->SetVsyncStatus(engineVsyncEnabled);
		}

		if (engineVsyncEnabled) {
			ImGui::BeginDisabled();
		}
		static bool engineFpsLimitEnabled = true;
		ImGui::Checkbox("Enable FPS Limit##1", &engineFpsLimitEnabled);
		int engineFpsLimit = 0;
		engineFpsLimit = App->GetEngineClock()->GetFpsLimit();
		ImGui::SliderInt("FPS Limit##1", &engineFpsLimit, 10, 240);
		if (engineVsyncEnabled) {
			ImGui::EndDisabled();
		}

		App->GetEngineClock()->SetFpsLimit(engineFpsLimit);
		ImGui::Spacing();
		ImGui::SeparatorText("Game");
		bool gameVsyncEnabled = App->GetGameClock()->GetVsyncStatus();
		ImGui::Checkbox("Game Vsync enabled", &gameVsyncEnabled);
		if (gameVsyncEnabled != App->GetGameClock()->GetVsyncStatus()) {
			App->GetGameClock()->SetVsyncStatus(gameVsyncEnabled);
		}

		if (gameVsyncEnabled) {
			ImGui::BeginDisabled();
		}
		static bool gameFpsLimitEnabled = true;
		ImGui::Checkbox("Enable FPS Limit##2", &gameFpsLimitEnabled);
		int gameFpsLimit = 0;
		gameFpsLimit = App->GetGameClock()->GetFpsLimit();
		ImGui::SliderInt("FPS Limit##2", &gameFpsLimit, 10, 240);
		if (gameVsyncEnabled) {
			ImGui::EndDisabled();
		}

		App->GetGameClock()->SetFpsLimit(gameFpsLimit);
		ImGui::Unindent();
		
		bool grid = App->GetDebugDraw()->GetShouldRenderGrid();
		ImGui::SeparatorText("Editor settings");
		if (ImGui::Checkbox("Draw Grid", &grid)) {
			App->GetDebugDraw()->SetRenderGrid(grid);
		}
	}
	ImGui::End();
}