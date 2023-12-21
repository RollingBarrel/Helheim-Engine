#include "InspectorPanel.h"
#include "imgui.h"

InspectorPanel::InspectorPanel() : Panel("Inspector", false) {}

void InspectorPanel::Draw(int windowFlags)
{
	//ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_Once);
	ImGui::Begin(GetName(), &mOpen, windowFlags);
	// Everything must be drawn by the components.
	ImGui::End();
}