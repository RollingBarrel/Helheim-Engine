#include "InspectorPanel.h"
#include "imgui.h"
#include "Application.h"
#include "ModuleScene.h"

InspectorPanel::InspectorPanel() : Panel(INSPECTORPANEL, true) {}

void InspectorPanel::Draw(int windowFlags)
{
	ImGui::SetNextWindowPos(ImVec2(-100, 100), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_Once);
	ImGui::Begin(GetName(), &mOpen, windowFlags);
	App->GetScene()->DrawInspector();

	ImGui::End();
}